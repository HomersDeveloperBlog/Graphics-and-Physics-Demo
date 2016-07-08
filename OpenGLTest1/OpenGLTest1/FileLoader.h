#pragma once

#include <fstream>
#include <tuple>
#include <memory>
#include <string>
#include "assert.h"

using namespace std;

//number of chars read on success, zero on failure.
//non-null pointer to char buffer on success, null on failure
string ReadFileIntoBuffer(
	const string & i_strFilePath)
{
	assert(i_strFilePath.data());
	ifstream oInputFileStream(
		i_strFilePath.data(), 
		ifstream::in | ifstream::binary);
	
	size_t nLength = 0x0ULL;
	shared_ptr<char> pBuffer = nullptr;
	if(oInputFileStream)
	{
		//Get file size
		oInputFileStream.seekg(0, oInputFileStream.end);
		nLength = oInputFileStream.tellg();
		
		//Create a suitable buffer
		if(nLength)
		{
			pBuffer = shared_ptr<char>(
				new (nothrow) char[nLength], 
				default_delete<char[]>());

			if(pBuffer)
			{
				//Read from the file into the buffer
				oInputFileStream.seekg(0, oInputFileStream.beg);
				oInputFileStream.read(pBuffer.get(), nLength);
			}
			else
			{
				nLength = 0x0ULL;
			}
		}
		
		oInputFileStream.close();
	}

	return (pBuffer) //Construction via nullptr undefined behaviour
	 ? (string(pBuffer.get(), nLength)) 
	 : (string());
}

size_t WriteFileFromBuffer(
	const string & i_strFilePath,
	const shared_ptr<char> i_pContentBuffer,
	size_t i_nBufferByteCount)
{
	assert(i_strFilePath.data());
	assert(i_pContentBuffer && i_nBufferByteCount);

	if(i_nBufferByteCount
		&& i_pContentBuffer)
	{
		ofstream oOutputFileStream(
			i_strFilePath.data(), 
			ofstream::out | ofstream::binary);

		if(oOutputFileStream)
		{ //Write to the file from the buffer
			oOutputFileStream.write(i_pContentBuffer.get(), i_nBufferByteCount);
			oOutputFileStream.close();
		}
	}

	return i_nBufferByteCount;
}