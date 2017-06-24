/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Combinations.cpp              */
/* Description:     config index array for a Russell support plugin for Kate */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_CONFIG_COMBINATIONS_CPP_
#define PLUGIN_KATE_RUSSELL_CONFIG_COMBINATIONS_CPP_

#include "plugin_kate_russell.hpp"

namespace plugin {
namespace kate {
namespace russell {
namespace config {

	/****************************
	 *	Public members
	 ****************************/

	Combinations :: Combinations (const Ui* ui) :
	ui_ (ui),
	combinations_ (),
	combNames_(),
	index_ (0) {
	}
	Combinations :: ~ Combinations() {
	}

	void
	Combinations :: setSane()
	{
		if (combinations_.isEmpty() || (index_ < 0) || combNames_.isEmpty()) {
			combinations_.clear();
			combNames_.clear();
			index_ = 0;
			combinations_ << Indexes (ui_);
			combNames_ << defaultName_;
		}
		int delta = combNames_.count() - combinations_.count();
		if (delta > 0) {
			for (int i = 0; i < delta; ++ i) {
				combinations_ << Indexes (ui_);
			}
		} else if (delta < 0) {
			delta = -delta;
			for (int i = 0; i < delta; ++ i) {
				combNames_ << defaultName_;
			}
		}
		for (int i = 0; i < combinations_.count(); ++ i) {
			combinations_ [i].setSane (ui_->config_);
		}
	}
	void
	Combinations :: setUi (const Ui* ui) {
		ui_ = ui;
	}
	void
	Combinations :: synchronize (const bool add)
	{
		#ifdef DEBUG_CONFIG
		std :: cout << "Combinations :: synchronize()" << std :: endl;
		#endif
		index_ = ui_->combinations_->currentIndex();
		if (add) {
			combNames_ << ui_->combinations_->itemText (index_);
			combinations_ << Indexes (combinations_ [index_], ui_);
		} else {
			combNames_.removeAt (index_);
			combinations_.removeAt (index_);
		}
		#ifdef DEBUG_CONFIG
		showToCout();
		#endif
	}
	void
	Combinations :: choose (const int index) {
		index_ = index;
	}
	void
	Combinations :: updateName (const int index)
	{
		if (ui_ == NULL) {
			return;
		}
		if (ui_->combinations_ == NULL) {
			return;
		}
 		index_ = index;
		combNames_ [index_] = ui_->combinations_->itemText (index_);
		#ifdef DEBUG_CONFIG
		showToCout();
		#endif
	}
	void
	Combinations :: readSessionConfig (const KConfigGroup& configGroup)
	{
		Session :: read (configGroup, i18n("Combination"), index_);
		Session :: readList (configGroup, i18n("CombinationNames"), combNames_, defaultName_);
		const int count = configGroup.readEntry (i18n("CombinationCount"), -1);
		combinations_.clear();
		if (count == 0 ) {
			combinations_.append (Indexes (ui_));
		} else {
			for (int i = 0; i < count; ++ i) {
				combinations_.append (Indexes (configGroup, i, ui_));
			}
		}
		setSane();
		#ifdef DEBUG_CONFIG
		showToCout();
		#endif
	}
	void
	Combinations :: writeSessionConfig (KConfigGroup& configGroup) const
	{
		Session :: write (configGroup, i18n("Combination"), index_);
		Session :: writeList (configGroup, i18n("CombinationNames"), combNames_);
		configGroup.writeEntry (i18n("CombinationCount"), combinations_.size());
		for (int i = 0; i < combinations_.size(); ++ i) {
			combinations_ [i].writeSessionConfig (configGroup, i);
		}
		#ifdef DEBUG_CONFIG
		showToCout();
		#endif
	}

	Indexes&
	Combinations :: combination() {
		return combinations_ [index_];
	}

	void
	Combinations :: showToCout() const
	{
		std :: cout << "Combinations :: showToCout()" << std :: endl;
		std :: cout << "\tindex_ = " << index_ << std :: endl;
		std :: cout << "\tcount = " << combinations_.size() << std :: endl;

		for (int i = 0; i < combinations_.size(); ++ i) {
			std :: cout << "combination " << qPrintable (combNames_ [i]) << " - " << i << ":" << std :: endl;
			combinations_ [i].showToCout();
		}
	}

	/****************************
	 *	Private members
	 ****************************/

	/*************************************
	 *	Private static attributes
	 *************************************/

	const QString Combinations :: defaultName_ = i18n("combination 1");
}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_CONFIG_COMBINATIONS_CPP_ */

