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

#ifndef IEX_PCAP_PRODUCER_HPP
#define	IEX_PCAP_PRODUCER_HPP

#include <io/interface>
#include <pcap/pcap.h>
#include <glog/logging.h>
#include <string>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include "../protocol"

namespace iex {
	namespace pcap {
		/** mock pcap packet to compute only the length, eventually 
		 * all fields are discarded
		 * @sa iex::transport::header iex::Transport
		 */
		struct packet {
			uint8_t ethernet_frame[14];  //!< source,dest mac address + 2byte type 
			uint8_t ipv4[20]; 	         //!< IP address header, we don't care 
			uint8_t udp[8]; 	         //!< UDP frame: src,dst, len,chk sum 
		}__attribute__((packed)) ;
}}

namespace iex {
	/** extracts data from pcap stream, then after disassembling packets delegates
	 * them to IexProtocolProducer for further processing  
	 * you must link against pcap library -lpcap
	 * \ingroup IEX
	 * @see io::Producer
	 * @see io::Consumer
	 * @see io::execute
	 */
	template <class Consumer> struct PcapProducer : public Transport<Consumer> {
		using type = PcapProducer<Consumer>;
		using duration = typename Consumer::duration;
		/** 
		 * @param path IEX data stream in pcap format as layed out in IEX protocol 
		 */ 
		PcapProducer(const std::string& path,duration heart_beat ) : input(path){
			fd = std::fopen( path.c_str(), "r"); // pcap_close will close this fd as well!
			this->heart_beat_interval = heart_beat;
		}
		PcapProducer( FILE *fd, duration heart_beat ) : fd(fd) {
			this->heart_beat_interval = heart_beat;
		}

		/**
		 * opens and reads input file, or exist with fatal ERROR if data stream is not Link Layer type 1
		 * Ethernet encapsulated UDP packets
		 * This method is usually called back by io::execute
		 * @see io::Producer
		 * @see io::Consumer
		 * @see io::execute
		 */
		void run_impl(){
			pcap_t *fd_ = pcap_fopen_offline( fd, errbuf );

			if( fd_ == NULL ) 
				DLOG(ERROR) << "couldn't open file: " << input << " error: " << std::string(errbuf);
			int layer_type =   pcap_datalink( fd_ );
				DLOG(INFO) << "Link Layer type: " << layer_type;
			if( layer_type != 1 )
				DLOG(FATAL) << "only ethernet frames are handled!!!";
			if(  pcap_loop( fd_, 0, pcap_handler, (uint8_t*)this ) < 0)
				DLOG(FATAL) << "pcap_loop() failed: " << pcap_geterr(fd_);
			pcap_close(fd_);
		}

		private:
		static void pcap_handler(uint8_t *ptr, const struct pcap_pkthdr* h, const uint8_t * start){
			type* producer =  static_cast<type*>((void*)ptr);
			const iex::transport::header* segment = static_cast<iex::transport::header*>(
						(void*)( start + sizeof(iex::pcap::packet)) );

			producer->transport_handler( segment );
		}

		private:
			char  errbuf[PCAP_ERRBUF_SIZE];
			const std::string input;
			std::FILE* fd;
	};
}
#endif

