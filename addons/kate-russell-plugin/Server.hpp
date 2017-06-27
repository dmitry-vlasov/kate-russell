/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_mdl_Server.hpp                       */
/* Description:     an mdl server launcher                                   */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_MDL_SERVER_HPP_
#define PLUGIN_KATE_RUSSELL_MDL_SERVER_HPP_

#include <QProcess>
#include <QTcpSocket>

#include "russell.dpp"

namespace plugin {
namespace kate {
namespace russell {
namespace mdl {

class Server {
public :
	Server (View*, const Config*);
	virtual ~ Server();

	bool start();
	bool stop();
	KProcess* process();
	bool isRun() const;

private :
	bool lookForRunning();

	View*         view_;
	const Config* config_;
	KProcess*     process_;
	bool          isRun_;
};

}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_MDL_SERVER_HPP_ */

