/*
 *   ALL RIGHTS RESERVED.
 *   _________________________________________________________________________________
 *   NOTICE: All information contained  herein is, and remains the property  of  Varga
 *   Consulting and  its suppliers, if  any. The intellectual and  technical  concepts
 *   contained herein are proprietary to Varga Consulting and its suppliers and may be
 *   covered  by  Canadian and  Foreign Patents, patents in process, and are protected
 *   by  trade secret or copyright law. Dissemination of this information or reproduc-
 *   tion  of  this  material is strictly forbidden unless prior written permission is
 *   obtained from Varga Consulting.
 *
 *   Copyright © <2017-2025> Varga Consulting, Toronto, On     info@vargaconsulting.ca
 *   _________________________________________________________________________________
 */
#include  <cstdint>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#ifndef IEX_TRANSPORT_v125
#define IEX_TRANSPORT_v125


namespace iex { namespace transport {
		// iex->message_count == 0 && iex->payload_length == 0 => heart_beat

		/** Outbound Segments are sent by sources to listeners. Each Outbound Segment contains 
		 * zero or more messages sent to listeners, described by the following IEX-TP Header.
		 * Each Outbound Segment consists of a header and a payload that carries the actual data 
		 * stream represented as a series of Message Blocks.
		 */ 
		struct header { 				// 40 bytes
			uint8_t  version; 					//!< 0x1 Version of Transport specification
			uint8_t  res; 						//!< reserved byte
			uint16_t protocol_id; 				//!< IEX_DEEPS_PROTOCOL_ID | IEX_TOPS_PROTOCOL_ID
			uint32_t channel_id; 				//!< Identifies the stream of bytes/sequenced messages
			uint32_t session_id; 				//!< Identifies the session
			uint16_t payload_length; 			//!< Byte length of the payload
			uint16_t message_count; 			//!< Number of messages in the payload
			int64_t  stream_offset; 			//!< Byte offset of the data stream
			int64_t  message_sequence; 			//!< Sequence of the first message in the segment
			int64_t  time; 						//!< Send time of segment
		}__attribute__((packed));
		static_assert( sizeof(header) == 40, "not aligned to byte!!!");
}}


#endif
