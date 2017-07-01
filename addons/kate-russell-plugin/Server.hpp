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
	static bool start();
	static bool stop();
	static KProcess& process() { return mod().process_; }
	static bool is_running() { return mod().process_.state() == QProcess::Running; }

private :
	static Server& mod() { static Server server; return server; }
	KProcess process_;
};

}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_MDL_SERVER_HPP_ */

