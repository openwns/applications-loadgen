/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef APPLICATIONS_SESSION_CLIENT_EMAIL_HPP
#define APPLICATIONS_SESSION_CLIENT_EMAIL_HPP

#include <APPLICATIONS/session/client/Session.hpp>

namespace applications { namespace session { namespace client {

      /* EmailStates are used to see Sessionphase */
      enum ClientEmailState
	{
	  reading = 1,
	  receiving = 2,
	  requestpop3ok = 3,
	  waitforpop3ok = 4,
	  waitforusernameok = 5,
	  waitforpasswordok = 6,
	  writing = 7,
	  sending = 8,
	  requestsmtpok = 9,
	  waitforsmtpok = 10,
	  readlastemail = 11
	};

      class Email :
    public applications::session::client::Session
      {
      public:
	Email(const wns::pyconfig::View& _pyco);
	~Email();

	void
	onData(const wns::osi::PDUPtr& _pdu);

	void
	onTimeout(const Timeout& _t);

      private:
	/* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 121-122,
	   section 10.7" the mean number of sent emails every day is 14. */
	wns::distribution::Distribution* numberOfEmailsDistribution;
	int numberOfEmails;

	/* emailChoice is used to decide between large or small emailsize.
	   Therefor a random value between 0.0 and 1.0 is Uniform distributed.*/
	wns::distribution::Distribution* emailSizeChoiceDistribution;

	/* 80% of all emails have an average size of 22.7KB because they have no attachments.
	   The values are Cauchy distributed. This is according to "IEEE 802.16m Evaluation
	   Methodology Document(2009), Page 121-122, section 10.7". */
	wns::distribution::Distribution* smallEmailSizeDistribution;
	/* 20% of all emails have attachments and so an avreage size of 227KB. The values
	   are also Cauchy distributed. This is according to "IEEE 802.16m Evaluation
	   Methodology Document(2009), Page 121-122, section 10.7". */
	wns::distribution::Distribution* largeEmailSizeDistribution;


	/* According to "IEEE 802.16m Evaluation Methodology Document(2009),
	   Page 121-122, section 10.7", the mean readingtime of one received
	   email is 60sek.. The values are Pareto distributed. */
	wns::distribution::Distribution* emailReadingTimeDistribution;

	/* According to "IEEE 802.16m Evaluation Methodology Document(2009),
	   Page 121-122, section 10.7", the mean writingtime for one email
	   is 120sek.. The values are Pareto distributed. */
	wns::distribution::Distribution* emailWritingTimeDistribution;

	/* emailState1: used for receiving emails */
	applications::session::client::ClientEmailState emailState1;
	/* emailState2: used for sending emails */
	applications::session::client::ClientEmailState emailState2;

	bool lastEmailSent;

      };
    } // nmaespace client
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_CLIENT_EMAIL_HPP
