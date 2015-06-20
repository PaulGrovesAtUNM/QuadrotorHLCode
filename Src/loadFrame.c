#include "quadComm.h"
#include "quadCommands.h"

#include "loadFrame.h"

FRAME frame;

// Processes bytes in our ring buffer and pulls out a frame.
//  Returns TRUE if there is a valid frame.
//  FALSE if there is not a valid frame in the Ring Buffer.
char loadFrame(void)
{
    int i;
    int sIndex, eIndex; //SOF / EOF indexes.
    int nBuf; //Bytes in the buffer.

    // Loop through the available data while we have enough bytes to form a frame.
    while ( bytesAvailable() >= 8 )
    {
        nBuf = bytesAvailable();

        // Find our first SOF byte.
        sIndex = findInBuffer(  0, nBuf, SOF );
        if ( sIndex == -1 ) //No SOF in the entire buffer?
        {
            skipBytes(nBuf); //Discard the entire buffer.
            continue;        //And try again.
        }

        // Make sure the SOF is the first entry in our Ring Buffer
        nBuf = skipBytes(sIndex);

        // If we don't have enough characters left in the buffer for a frame
        if ( nBuf < 8 )
            return 0; // Then we're done.

        // Now we need to make sure we don't have another SOF.
        // We shouldn't have another for at least 8 bytes.
        sIndex = findInBuffer( 1, 7, SOF );
        if ( sIndex != -1 ) // We have another SOF in the next 7 characters.
        {
            skipBytes( sIndex ); //Skip up until then
            continue;
        }

        // We don't have another SOF, do we have a valid EOF?
        eIndex = findInBuffer( 1, 7, EOFm );

        if (eIndex != 6) //EOF should be the 8th byte out. Bad Frame! Bad!
        {
            // We will discard byte up to the EOF if it was found...
            if ( eIndex >= 0 )
                skipBytes( eIndex + 1 ); //Discard up to and including the EOF.
            else
                skipBytes( 8 ); //No EOF was found at all. Discard next 8 bytes.
            continue;
        }
        // We have a valid frame.
        frame.prefix = getByte(); //SOF byte
        // Get the command byte
        frame.command = getByte();
        getBytes(frame.data, 5); //Data bytes.
        frame.suffix = getByte(); //EOF byte

        return 1;
    }
    return 0; //No valid frame present.
}
