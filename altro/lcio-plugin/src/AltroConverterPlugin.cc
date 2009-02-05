#include "eudaq/AltroConverterPlugin.hh"
#include "eudaq/Exception.hh"
#include "eudaq/RawDataEvent.hh"
#include "eudaq/Logger.hh"

#include <IMPL/LCEventImpl.h>
#include <IMPL/TrackerRawDataImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCIO.h>

#include <iostream>
#include <cmath>

namespace eudaq
{

AltroConverterPlugin const AltroConverterPlugin::m_altroconverterplugininstance;

UCharBigEndianVec::UCharBigEndianVec(std::vector<unsigned char> const & datavec) :
    bytedata(datavec) {}

unsigned long long int UCharBigEndianVec::Get40bitWord(unsigned int index40bit,
							  unsigned int offset32bit) const
{
    // check position in data stream
    if ( Get32bitWord(offset32bit - 8) != 0xFFFFFFFF )
	    EUDAQ_WARN("AltroConverterPlugin::Get40bitWord: Suspicious position in data stream, altro sequence does not start 8 words after rcu header");	

    unsigned int index8bit = offset32bit*4 + index40bit*5;
    
    // The 32  bit words have big endian order. The 40 bit word is always contained 
    // in 2 consecutive 32 bit words,
    // the most significant bytes being in the second word (big endian 32 bit sequence).
    // This means the 5 bytes are in a consecutive
    // sequence in the big endian data stream, but in reverse order. We just have to pick them out 
    // and place them in the long long word

    unsigned long long int retval = 
	static_cast<unsigned long long int>( bytedata[ index8bit ] )  |
	static_cast<unsigned long long int>( bytedata[ index8bit + 1 ] ) << 8  |
	static_cast<unsigned long long int>( bytedata[ index8bit + 2 ] ) << 16 |
	static_cast<unsigned long long int>( bytedata[ index8bit + 3 ] ) << 24 |
	static_cast<unsigned long long int>( bytedata[ index8bit + 4 ] ) << 32 ;
    return retval;
}

unsigned int UCharBigEndianVec::Get32bitWord(unsigned int index32bit) const
{
    unsigned int index8bit = index32bit*4;

    /// read the 4 bytes, least significant first (big endian byte sequence)
    unsigned int retval = 
	static_cast<unsigned int>( bytedata[ index8bit     ] )       |
	static_cast<unsigned int>( bytedata[ index8bit + 1 ] ) <<  8 |
	static_cast<unsigned int>( bytedata[ index8bit + 2 ] ) << 16 |
	static_cast<unsigned int>( bytedata[ index8bit + 3 ] ) << 24;

    return retval;
}

unsigned short UCharBigEndianVec::Get10bitWord(unsigned int index10bit,
					     unsigned int offset32bit) const
{
    // The way to get the bits out of the data stream is a sliding window algorithm.
    // The sequence is always contained in two consecutive bytes. First the two relevant bytes
    // are copied to a 16 bit word, then the 10 bit mask is placed at the specific position.
    // The position depends on the 10 bit index (the second word is shifted by 2 bits, the third 
    // by 4 bits and the fourth by 6 bits. The fifth word is shifted by 8 bits, which means no
    // shift, but index8bit is increased by 1).
    // Afterwords the ten bits are shifted to the end of the 16 bit word.

    // Calculate the 8 bit index. Note that the first part is an integer division, which 
    // automatically generates the correct index ( 1*10/8=1 , 2*10/8=2, 3*10/8=3, 4*10/8=5 etc)
    unsigned int   index8bit = index10bit * 10 / 8 + offset32bit*4;

    // Compose the 2-byte word onto which the mask will be applied.
    // The least significant is the first byte in the bytedata vector.
    unsigned short data16bit = bytedata[index8bit] | (bytedata[index8bit + 1] << 8) ;

    // Calculate how far to shift the window, i. e. relative alignment of 10-bit and 8-bit blocks
    unsigned short bitshift = (index10bit % 4) * 2;
    unsigned short mask = 0x3FF << bitshift; // 10-bit mask, shifted

    return (data16bit & mask) >> bitshift; // apply the mask and shift the result back to the LSB
    
}

lcio::LCEvent * AltroConverterPlugin::GetLCIOEvent( eudaq::Event const * eudaqevent ) const
{
    //try to cast the eudaq event to RawDataEvent
    eudaq::RawDataEvent const *rawdataevent = dynamic_cast<eudaq::RawDataEvent const *>(eudaqevent);
    if (rawdataevent == 0) //cast failed, throw an exception
	EUDAQ_THROW(std::string("AltroConverterPlugin::GetLCIOEvent: Error") + 
		    " given event is not an eudaq::RawDataEvent");
    
    //check type of RawDataEvent
    if (rawdataevent->GetType() != "AltroEvent")
	EUDAQ_THROW(std::string("AltroConverterPlugin::GetLCIOEvent: Error") + 
		    " given event is not an AltroEvent");
 
    lcio::LCEventImpl * lcioevent = new lcio::LCEventImpl;
//    lcioevent->setEventNumber( eudaqevent->GetEventNumber () ); done later
    lcioevent->setRunNumber(    eudaqevent->GetRunNumber () );
    lcioevent->setDetectorName( "TPC with Pads and Altro");
    lcioevent->setTimeStamp( eudaqevent->GetTimestamp() );


    lcio::LCCollectionVec * altrocollection = new lcio::LCCollectionVec(lcio::LCIO::TRACKERRAWDATA);

    // loop all data blocks
    for (size_t block = 0 ; block < rawdataevent->NumBlocks(); block++)
    {
	std::vector<unsigned char> bytedata_ = rawdataevent->GetBlockUChar(block);
	UCharBigEndianVec altrodatavec(bytedata_);

	// interpret the byte sequence

	// test integrity
	unsigned int eventlength = altrodatavec.Get32bitWord(0);
	if ( eventlength+1 != altrodatavec.Size())
	{
	    std::cout << "event length = "<<( eventlength+1)*4<< " data size " << altrodatavec.Size() << std::endl; 
	    EUDAQ_WARN("AltroConverterPlugin::GetLCIOEvent: Wrong event length given in raw data");
	}

	unsigned int headerlength = altrodatavec.Get32bitWord(1);
	if ( (headerlength == 4) )
	{
	    EUDAQ_WARN("AltroConverterPlugin::GetLCIOEvent: Suspicious header length given in raw data");
	    headerlength = 5; // this should be the correct size
	}

	// the third word is the block ID, it has to be 0x22222222
	unsigned int blockID = altrodatavec.Get32bitWord(2);
	if (blockID != 0x22222222)
	{
	    EUDAQ_THROW("AltroConverterPlugin::GetLCIOEvent: Error reading event header");
	}

	// read the TLU event number (7th word)
	unsigned int tlu_eventnumber = altrodatavec.Get32bitWord(6);
	if (tlu_eventnumber != 0)
	{
	       lcioevent->setEventNumber( tlu_eventnumber  );
	}
	else // tlu event number is not set, use the software number
	{
	       lcioevent->setEventNumber( eudaqevent->GetEventNumber () );
	} 

	// the start position of the rcu block in 32bit bords
	unsigned int rcublockstart = 7;
	while ( rcublockstart < altrodatavec.Size() )
	{
	    // the first word in the rcu block is its length
	    unsigned int rcublocklength = altrodatavec.Get32bitWord(rcublockstart);
//	    std::cout << "DEBUG: rcublocklength = "<<rcublocklength<< " at rcublockstart" 
//		      << rcublockstart << std::endl;

	    // check that we have the correct position of the rcu block, third word has to be 0xFFFFFFFF
	    if (  altrodatavec.Get32bitWord(rcublockstart +2 ) != 0xFFFFFFFF )
	    {
		std::cerr << "invalid rcu identifier 0x"<<std::hex << altrodatavec.Get32bitWord(rcublockstart +2 )
			  <<std::dec << std::endl;
		EUDAQ_THROW("Invalid RCU identifier");
	    }

	    // the rcu block ends with the trailer, 
	    // the last trailer word contains the trailer length (bits [6:0] )
	    unsigned int rcutrailerlength =  altrodatavec.Get32bitWord(rcublockstart + rcublocklength ) 
                                             & 0x7F ;
//	    std::cout << "DEBUG: rcutrailerlength "<<rcutrailerlength << std::endl;

	    // the number of 40 bit words is the first entry in the rcu trailer
	    unsigned int n40bitwords = altrodatavec.Get32bitWord(rcublockstart 
						    + rcublocklength 
						    - rcutrailerlength + 1);

//	    std::cout << "DEBUG: rcu block contains "<<n40bitwords<<" 40bit words"<< std::endl;


	    // read the sequence of 40 bit altro words backward. It is made up of blocks
	    // ending with a trailer word

	    // the position of the next trailer word to read
            // use a signed int, it will become negative if there are no more words to read
	    int altrotrailerposition = n40bitwords-1;
//	    std::cout << "DEBUG: altrotrailerposition" << altrotrailerposition << std::endl;

//	    // dump the 10 bit hex data
//	    for (unsigned int i=0; i < n40bitwords ; i++)
//	    {
//		std::cout << std::hex << std::setfill('0')
//			  << std::setw(3)<< altrodatavec.Get10bitWord(i+3, rcublockstart + 10 )<<" "
//			  << std::setw(3)<< altrodatavec.Get10bitWord(i+2, rcublockstart + 10 )<<" "
//			  << std::setw(3)<< altrodatavec.Get10bitWord(i+1, rcublockstart + 10 )<<" "
//			  << std::setw(3)<< altrodatavec.Get10bitWord(i  , rcublockstart + 10 )
//			  <<std::dec<<std:: endl;
//	    }

//	    // dump the 40 bit hex data
//	    std::cout <<"blip"<<std::endl;
//	    for (unsigned int i=0; i <= n40bitwords ; i++)
//	    {
//		std::cout << std::hex << std::setfill('0')
//			  << std::setw(10)<< altrodatavec.Get40bitWord(i, rcublockstart + 10 )
//			  <<std::dec<<std:: endl;
//	    }

//	    std::cout <<"blub"<<std::endl;
//	    // dump the 32 bit hex data
//	    for (unsigned int i= rcublockstart ; i <= rcublockstart + rcublocklength ; i++)
//	    {
//		std::cout << std::hex << std::setfill('0')
//			  << std::setw(8)<< altrodatavec.Get32bitWord(i)
//			  <<std::dec<<std:: endl;
//	    }

	    while (altrotrailerposition > 0)
	    {
		unsigned long long int altrotrailer 
		    = altrodatavec.Get40bitWord(altrotrailerposition, rcublockstart +10 );

		// test if we realy have the altro trailer word
		if ( (altrotrailer & 0xFFFC00F000ULL) != 0xAAA800A000ULL )
		{
		    std::cerr << "error wrong altro trailer word 0x"<< std::hex << altrotrailer << std::dec << std::endl;
		    EUDAQ_THROW("Invalid Altro trailer word");
		}

		unsigned int n10bitwords = (altrotrailer & 0x3FF0000) >> 16;
		unsigned int channelnumber = altrotrailer & 0xFFF;
		
//		std::cout <<"DEBUG: altro block on channel "<< channelnumber <<" contains " 
//			  <<n10bitwords << " 10bit words"<< std::endl;

		// loop all pulse blocks, starting with the last 10 bit word
		int index10bit = n10bitwords - 1;
		while (index10bit > 0)
		{
		    // length of this data record, it also counts this length word
		    // and the timestamp word, so the number of data samples is length - 2
		    unsigned short length    = altrodatavec.Get10bitWord( index10bit ,
									  rcublockstart +10);
		    // the length word also coints itself and the timestamp
		    // so number of data samples is two less
		    unsigned short ndatasamples = length - 2;
		    // timestamp is the next to last word
		    unsigned short timestamp = altrodatavec.Get10bitWord( index10bit - 1 ,
									  rcublockstart +10);

//		    std::cout <<"DEBUG: found pulse with "<< ndatasamples 
//			      <<" ndatasamples at time index " << timestamp 
//			      <<" on channel " << channelnumber << std::endl;

		    // the time stamp in the data stream corresponds to the last sample
		    // in lcio it has to be the first sample
		    timestamp -= (ndatasamples - 1);

		    // create the lcio data record
		    lcio::TrackerRawDataImpl *altrolciodata=new lcio::TrackerRawDataImpl;

		    altrolciodata->setCellID0(channelnumber);
		    altrolciodata->setCellID1(block);
		    altrolciodata->setTime(timestamp);
		    
//		    std::cout <<"DEBUG: found pulse with "<< ndatasamples 
//			      <<" ndatasamples at time index " << timestamp 
//			      <<" on channel " << channelnumber << std::endl;

		    // fill the data samples into a vactor and add it to the lcio raw data
		    lcio::ShortVec datasamples( ndatasamples );
		    for ( unsigned int sample = index10bit - length + 1 , i = 0 ;
			  sample < static_cast<unsigned int>(index10bit) - 1 ;
			  sample ++ , i++  )
		    {
			datasamples[ i ] = altrodatavec.Get10bitWord( sample, rcublockstart +10 );
		    }
		    altrolciodata->setADCValues(datasamples);

		    // add the TrackerRawData to the lcio collection
		    altrocollection->addElement(altrolciodata);

		    // set index to the next 10 bit word to read
		    index10bit -= length;
		} // while (index10bit > 0)

		// calculate the number of 40bit words.
		// It is n10bitwords/4, rounded up (so we have to perform a floating point division)
		// plus the altro trailer word.
		unsigned int n40bitwords_in_altroblock = static_cast<unsigned int>(
		                                 std::ceil(static_cast<double>(n10bitwords)/4.)+1);

		// calculate the position of the next altro block trailer word
		altrotrailerposition -= n40bitwords_in_altroblock;
	    } // while (altrotrailerposition > 0)
	    
	    // calculate the position of the next rcu block
	    rcublockstart += rcublocklength +1;
	} // while ( rcublockstart < (bytedata.size() / 4 ))

    }// for (eudaq data block)
	

    // If the collection is not empty, delete the empty collection and event
    // and return 0
    if ( altrocollection->getNumberOfElements() == 0 )
    {
	delete altrocollection;
	delete lcioevent;
	return 0;
    }

    // If the collection is not empty, add the collection to the event and return the event
    lcioevent->addCollection(altrocollection,"AltroRawData");
    return lcioevent;
}

StandardEvent * AltroConverterPlugin::GetStandardEvent( eudaq::Event const * eudaqevent ) const
{
    StandardEvent * se = new StandardEvent;
    se->b = eudaqevent->GetEventNumber ();

    return se;
}

} //namespace eudaq