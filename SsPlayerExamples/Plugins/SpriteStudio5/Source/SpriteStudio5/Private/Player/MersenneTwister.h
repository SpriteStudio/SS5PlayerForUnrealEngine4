#ifndef __MERSENNETWISTER__
#define __MERSENNETWISTER__

/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   This C++ porting is done by stlalv on November 12, 2010. 
   e-mail:stlalv @ nifty.com (remove space)
   Original C code is found at http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/emt19937ar.html as mt19937ar.tgz
*/
#include <vector>
using namespace std;

class CMersenneTwister {
private:
	const int32             m_iN          ;
	const int32             m_iM          ;
	const int32             m_ulMATRIX_A  ;
	const uint32            m_ulUPPER_MASK;
	const uint32            m_ulLOWER_MASK;
	vector<uint32>          m_veculMT     ;
	int32                   m_iMTI        ;
public:
	CMersenneTwister()
	: m_iN          (624)
	, m_iM          (397)
	, m_ulMATRIX_A  (0x9908b0dfUL)	// constant vector a
	, m_ulUPPER_MASK(0x80000000UL)	// most significant w-r bits
	, m_ulLOWER_MASK(0x7fffffffUL)	// least significant r bits
	{
		m_veculMT.resize(m_iN);
		m_iMTI = m_iN + 1;			// mti==N+1 means mt[N] is not initialized
		uint32 aulInit[4] = { 0x123, 0x234, 0x345, 0x456 };	// use default seed
		init_by_array(aulInit, sizeof aulInit / sizeof aulInit[0]);
	}

	~CMersenneTwister(){
    	m_veculMT.clear();
	}


	// initializes mt[N] with a seed
	void init_genrand(uint32 ulSeed) {
		m_veculMT[0] = ulSeed & 0xffffffffUL;
		{for (m_iMTI = 1; m_iMTI < m_iN; m_iMTI++) {
			m_veculMT[m_iMTI] = (1812433253UL * (m_veculMT[m_iMTI - 1] ^ (m_veculMT[m_iMTI - 1] >> 30)) + m_iMTI); 
			// See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
			// In the previous versions, MSBs of the seed affect
			// only MSBs of the array mt[].
			// 2002/01/09 modified by Makoto Matsumoto
			m_veculMT[m_iMTI] &= 0xffffffffUL;	// for >32 bit machines
		}}
	}
	// initialize by an array with array-length
	// init_key is the array for initializing keys
	// key_length is its length
	void init_by_array(uint32 aulInitKey[], int32 iKeyLength) {
		init_genrand(19650218UL);
		int32 iI = 1;
		int32 iJ = 0;
		{for (int32 k = m_iN > iKeyLength ? m_iN : iKeyLength; k; k--) {
			m_veculMT[iI] = (m_veculMT[iI] ^ ((m_veculMT[iI - 1] ^ (m_veculMT[iI - 1] >> 30)) * 1664525UL))
			  + aulInitKey[iJ] + iJ; /* non linear */
			m_veculMT[iI] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
			iI++; iJ++;
			if (iI >= m_iN) { m_veculMT[0] = m_veculMT[m_iN - 1]; iI = 1; }
			if (iJ >= iKeyLength) { iJ = 0; }
		}}
		{for (int32 k = m_iN - 1; k; k--) {
			m_veculMT[iI] = (m_veculMT[iI] ^ ((m_veculMT[iI - 1] ^ (m_veculMT[iI - 1] >> 30)) * 1566083941UL)) - iI; // non linear
			m_veculMT[iI] &= 0xffffffffUL;	// for WORDSIZE > 32 machines
			iI++;
			if (iI >= m_iN) { m_veculMT[0] = m_veculMT[m_iN - 1]; iI = 1; }
		}}
		m_veculMT[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
	}
	// generates a random number on [0,0xffffffff]-interval
	uint32 genrand_uint32() {
		uint32 aulMag01[2] = { 0x0UL, (uint32)m_ulMATRIX_A };
		// mag01[x] = x * MATRIX_A  for x=0,1
		if (m_iMTI >= m_iN) {
			// generate N words at one time
			if (m_iMTI == m_iN+1) {
				/* if init_genrand() has not been called, */
				init_genrand(5489UL); /* a default initial seed is used */
			}
			int32 iKK = 0;
			{for (; iKK < m_iN - m_iM; iKK++) {
				uint32 ulY = (m_veculMT[iKK] & m_ulUPPER_MASK) | (m_veculMT[iKK + 1] & m_ulLOWER_MASK);
				m_veculMT[iKK] = m_veculMT[iKK + m_iM] ^ (ulY >> 1) ^ aulMag01[ulY & 0x1UL];
			}}
			{for (; iKK < m_iN - 1; iKK++) {
				uint32 ulY = (m_veculMT[iKK] & m_ulUPPER_MASK) | (m_veculMT[iKK + 1] & m_ulLOWER_MASK);
				m_veculMT[iKK] = m_veculMT[iKK + (m_iM - m_iN)] ^ (ulY >> 1) ^ aulMag01[ulY & 0x1UL];
			}}
			uint32 ulY = (m_veculMT[m_iN - 1] & m_ulUPPER_MASK) | (m_veculMT[0] & m_ulLOWER_MASK);
			m_veculMT[m_iN - 1] = m_veculMT[m_iM - 1] ^ (ulY >> 1) ^ aulMag01[ulY & 0x1UL];
			m_iMTI = 0;
		}
		uint32 ulY = m_veculMT[m_iMTI++];
		// Tempering
		ulY ^= (ulY >> 11)               ;
		ulY ^= (ulY <<  7) & 0x9d2c5680UL;
		ulY ^= (ulY << 15) & 0xefc60000UL;
		ulY ^= (ulY >> 18)               ;
		return ulY;
	}
	// generates a random floating point number on [0,1]
	double genrand_real1() {
		return genrand_uint32() * (1.0 / 4294967295.0);	// divided by 2^32-1
	}
	// generates a random floating point number on [0,1)
	double genrand_real2() {
		return genrand_uint32() * (1.0 / 4294967296.0);	// divided by 2^32
	}
	// generates a random integer number from 0 to N-1
	int32 genrand_N(int32 iN) {
		return (int32)(genrand_uint32() * (iN / 4294967296.0));
	}
};




#endif

