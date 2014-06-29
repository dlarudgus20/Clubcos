// Copyright (c) 2014, 임경현 (dlarudgus20)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file memory.c
 * @date 2014. 5. 5.
 * @author MINT64OS, by kkamagui (http://kkamagui.tistory.com/)
 *         modified by dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "memory.h"
#include "port.h"

static int csCalculateMetaBlockCount(uint32_t qwDynamicRAMSize);
static int csAllocationBuddyBlock(uint32_t qwAlignedSize);
static uint32_t csGetBuddyBlockSize(uint32_t qwSize);
static int csGetBlockListIndexOfMatchSize(uint32_t qwAlignedSize);
static int csFindFreeBlockInBitmap(int iBlockListIndex);
static void csSetFlagInBitmap(int iBlockListIndex, int iOffset, uint8_t bFlag);
static bool csFreeBuddyBlock(int iBlockListIndex, int iBlockOffset);
static uint8_t csGetFlagInBitmap(int iBlockListIndex, int iOffset);

DynMemStruct g_DynMem;

void ckMemoryInitialize(void)
{
	// ckMemoryCheckSize()는 ckMain()에서 메모리 크기 점검을 위해 먼저 호출됨.

	uint32_t qwDynamicMemorySize;
	int i, j;
	uint8_t* pbCurrentBitmapPosition;
	int iBlockCountOfLevel, iMetaBlockCount;

	// 동적 메모리 영역으로 사용할 메모리 크기를 이용하여 블록을 관리하는데
	// 필요한 메모리 크기를 최소 블록 단위로 계산
	qwDynamicMemorySize = g_DynMem.DynMemSize;
	iMetaBlockCount = csCalculateMetaBlockCount( qwDynamicMemorySize );

	// 전체 블록 개수에서 관리에 필요한 메타블록의 개수를 제외한 나머지 영역에 대해서
	// 메타 정보를 구성
	g_DynMem.iBlockCountOfSmallestBlock =
		( qwDynamicMemorySize / DYN_MEMORY_MIN_SIZE ) - iMetaBlockCount;

	// 최대 몇 개의 블록 리스트로 구성되는지를 계산
	for( i = 0 ; ( g_DynMem.iBlockCountOfSmallestBlock >> i ) > 0 ; i++ )
	{
		//DO Nothing
		;
	}
	g_DynMem.iMaxLevelCount = i;

	// 할당된 메모리가 속한 블록 리스트의 인덱스를 저장하는 영역을 초기화
	g_DynMem.pbAllocatedBlockListIndex = ( uint8_t* ) DYN_MEMORY_START_ADDRESS;
	for( i = 0 ; i < g_DynMem.iBlockCountOfSmallestBlock ; i++ )
	{
		g_DynMem.pbAllocatedBlockListIndex[ i ] = 0xFF;
	}

	// 비트맵 자료구조의 시작 어드레스 지정
	g_DynMem.pstBitmapOfLevel = ( DynMemBitmap* ) ( DYN_MEMORY_START_ADDRESS +
		( sizeof( uint8_t ) * g_DynMem.iBlockCountOfSmallestBlock ) );
	// 실제 비트맵의 어드레스를 지정
	pbCurrentBitmapPosition = ( ( uint8_t* ) g_DynMem.pstBitmapOfLevel ) +
		( sizeof( DynMemBitmap ) * g_DynMem.iMaxLevelCount );
	// 블록 리스트 별로 루프를 돌면서 비트맵을 생성
	// 초기 상태는 가장 큰 블록과 자투리 블록만 존재하므로 나머지는 비어있는 것으로 설정
	for( j = 0 ; j < g_DynMem.iMaxLevelCount ; j++ )
	{
		g_DynMem.pstBitmapOfLevel[ j ].pbBitmap = pbCurrentBitmapPosition;
		g_DynMem.pstBitmapOfLevel[ j ].qwExistBitCount = 0;
		iBlockCountOfLevel = g_DynMem.iBlockCountOfSmallestBlock >> j;

		// 8개 이상 남았으면 상위 블록으로 모두 결합할 수 있으므로, 모두 비어있는 것으로 설정
		for( i = 0 ; i < iBlockCountOfLevel / 8 ; i++ )
		{
			*pbCurrentBitmapPosition = 0x00;
			pbCurrentBitmapPosition++;
		}

		// 8로 나누어 떨어지지 않는 나머지 블록들에 대한 처리
		if( ( iBlockCountOfLevel % 8 ) != 0 )
		{
			*pbCurrentBitmapPosition = 0x00;
			// 남은 블록이 홀수라면 마지막 한 블록은 결합되어 상위 블록으로 이동하지 못함
			// 따라서 마지막 블록은 현재 블록 리스트에 존재하는 자투리 블록으로 설정
			i = iBlockCountOfLevel % 8;
			if( ( i % 2 ) == 1 )
			{
				*pbCurrentBitmapPosition |= ( DYN_MEMORY_EXIST_FLAG << ( i - 1 ) );
				g_DynMem.pstBitmapOfLevel[ j ].qwExistBitCount = 1;
			}
			pbCurrentBitmapPosition++;
		}
	}

	// 블록 풀의 어드레스와 사용된 메모리 크기 설정
	g_DynMem.qwStartAddress = DYN_MEMORY_START_ADDRESS +
		iMetaBlockCount * DYN_MEMORY_MIN_SIZE;
	g_DynMem.qwEndAddress = qwDynamicMemorySize +
		DYN_MEMORY_START_ADDRESS;
	g_DynMem.qwUsedSize = 0;
}

/**
 *  동적 메모리 영역을 관리하는데 필요한 정보가 차지하는 공간을 계산
 *      최소 블록 단위로 정렬해서 반환
 */
static int csCalculateMetaBlockCount( uint32_t qwDynamicRAMSize )
{
	long lBlockCountOfSmallestBlock;
	uint32_t dwSizeOfAllocatedBlockListIndex;
	uint32_t dwSizeOfBitmap;
	long i;

	// 가장 크기가 작은 블록의 개수를 계산하여 이를 기준으로 비트맵 영역과
	// 할당된 크기를 저장하는 영역을 계산
	lBlockCountOfSmallestBlock = qwDynamicRAMSize / DYN_MEMORY_MIN_SIZE;
	// 할당된 블록이 속한 블록 리스트의 인덱스를 저장하는데 필요한 영역을 계산
	dwSizeOfAllocatedBlockListIndex = lBlockCountOfSmallestBlock * sizeof( uint8_t );

	// 비트맵을 저장하는데 필요한 공간 계산
	dwSizeOfBitmap = 0;
	for( i = 0 ; ( lBlockCountOfSmallestBlock >> i ) > 0 ; i++ )
	{
		// 블록 리스트의 비트맵 포인터를 위한 공간
		dwSizeOfBitmap += sizeof( DynMemBitmap );
		// 블록 리스트의 비트맵 크기, 바이트 단위로 올림 처리
		dwSizeOfBitmap += ( ( lBlockCountOfSmallestBlock >> i ) + 7 ) / 8;
	}

	// 사용한 메모리 영역의 크기를 최소 블록 크기로 올림해서 반환
	return ( dwSizeOfAllocatedBlockListIndex + dwSizeOfBitmap +
		DYN_MEMORY_MIN_SIZE - 1 ) / DYN_MEMORY_MIN_SIZE;
}

/**
 *  메모리를 할당
 */
void* ckMemoryAllocateBuddy( uint32_t qwSize )
{
	uint32_t qwAlignedSize;
	uint32_t qwRelativeAddress;
	long lOffset;
	int iSizeArrayOffset;
	int iIndexOfBlockList;

	// 메모리 크기를 버디 블록의 크기로 맞춤
	qwAlignedSize = csGetBuddyBlockSize( qwSize );
	if( qwAlignedSize == 0 )
	{
		return NULL;
	}

	// 만약 여유 공간이 충분하지 않으면 실패
	if( g_DynMem.qwStartAddress + g_DynMem.qwUsedSize +
			qwAlignedSize > g_DynMem.qwEndAddress )
	{
		return NULL;
	}

	// 버디 블록 할당하고 할당된 블록이 속한 블록 리스트의 인덱스를 반환
	lOffset = csAllocationBuddyBlock( qwAlignedSize );
	if( lOffset == -1 )
	{
		return NULL;
	}

	iIndexOfBlockList = csGetBlockListIndexOfMatchSize( qwAlignedSize );

	// 블록 크기를 저장하는 영역에 실제로 할당된 버디 블록이 속한 블럭 리스트의
	// 인덱스를 저장
	// 메모리를 해제할 때 블록 리스트의 인덱스를 사용
	qwRelativeAddress = qwAlignedSize * lOffset;
	iSizeArrayOffset = qwRelativeAddress / DYN_MEMORY_MIN_SIZE;
	g_DynMem.pbAllocatedBlockListIndex[ iSizeArrayOffset ] =
		( uint8_t ) iIndexOfBlockList;
	g_DynMem.qwUsedSize += qwAlignedSize;

	return ( void* ) ( qwRelativeAddress + g_DynMem.qwStartAddress );
}

/**
 *  가장 가까운 버디 블록의 크기로 정렬된 크기를 반환
 */
static uint32_t csGetBuddyBlockSize( uint32_t qwSize )
{
	long i;

	for( i = 0 ; i < g_DynMem.iMaxLevelCount ; i++ )
	{
		if( qwSize <= ( DYN_MEMORY_MIN_SIZE << i ) )
		{
			return ( DYN_MEMORY_MIN_SIZE << i );
		}
	}
	return 0;
}

/**
 *  버디 블록 알고리즘으로 메모리 블록을 할당
 *      메모리 크기는 버디 블록의 크기로 요청해야 함
 */
static int csAllocationBuddyBlock( uint32_t qwAlignedSize )
{
	int iBlockListIndex, iFreeOffset;
	int i;
	uint32_t eflag;

	// 블록 크기를 만족하는 블록 리스트의 인덱스를 검색
	iBlockListIndex = csGetBlockListIndexOfMatchSize( qwAlignedSize );
	if( iBlockListIndex == -1 )
	{
		return -1;
	}

	// 동기화 처리
	eflag = ckAsmGetEFlag();
	ckAsmCli();

	// 만족하는 블록 리스트부터 최상위 블록 리스트까지 검색하여 블록을 선택
	for( i = iBlockListIndex ; i< g_DynMem.iMaxLevelCount ; i++ )
	{
		// 블록 리스트의 비트맵을 확인하여 블록이 존재하는지 확인
		iFreeOffset = csFindFreeBlockInBitmap( i );
		if( iFreeOffset != -1 )
		{
			break;
		}
	}

	// 마지막 블록 리스트까지 검색했는데도 없으면 실패
	if( iFreeOffset == -1 )
	{
	  //  UnlockForSystemData( bPreviousInterruptFlag );
		return -1;
	}

	// 블록을 찾았으니 빈 것으로 표시
	csSetFlagInBitmap( i, iFreeOffset, DYN_MEMORY_EMPTY_FLAG );

	// 상위 블록에서 블록을 찾았다면 상위 블록을 분할
	if( i > iBlockListIndex )
	{
		// 검색된 블록 리스트에서 검색을 시작한 블록 리스트까지 내려가면서 왼쪽 블록은
		// 빈 것으로 표시하고 오른쪽 블록은 존재하는 것으로 표시함
		for( i = i - 1 ; i >= iBlockListIndex ; i-- )
		{
			// 왼쪽 블록은 빈 것으로 표시
			csSetFlagInBitmap( i, iFreeOffset * 2, DYN_MEMORY_EMPTY_FLAG );
			// 오른쪽 블록은 존재하는 것으로 표시
			csSetFlagInBitmap( i, iFreeOffset * 2 + 1, DYN_MEMORY_EXIST_FLAG );
			// 왼쪽 블록을 다시 분할
			iFreeOffset = iFreeOffset * 2;
		}
	}
   ckAsmSetEFlag(eflag);

	return iFreeOffset;
}

/**
 *  전달된 크기와 가장 근접한 블록 리스트의 인덱스를 반환
 */
static int csGetBlockListIndexOfMatchSize( uint32_t qwAlignedSize )
{
	int i;

	for( i = 0 ; i < g_DynMem.iMaxLevelCount ; i++ )
	{
		if( qwAlignedSize <= ( DYN_MEMORY_MIN_SIZE << i ) )
		{
			return i;
		}
	}
	return -1;
}

/**
 *  블록 리스트의 비트맵를 검색한 후, 블록이 존재하면 블록의 오프셋을 반환
 */
static int csFindFreeBlockInBitmap( int iBlockListIndex )
{
	int i, iMaxCount;
	uint8_t* pbBitmap;
	uint32_t* pqwBitmap;

	// 비트맵에 데이터가 존재하지 않는다면 실패
	if( g_DynMem.pstBitmapOfLevel[ iBlockListIndex ].qwExistBitCount == 0 )
	{
		return -1;
	}

	// 블록 리스트에 존재하는 총 블록의 수를 구한 후, 그 개수만큼 비트맵을 검색
	iMaxCount = g_DynMem.iBlockCountOfSmallestBlock >> iBlockListIndex;
	pbBitmap = g_DynMem.pstBitmapOfLevel[ iBlockListIndex ].pbBitmap;
	for( i = 0 ; i < iMaxCount ; )
	{
		// uint32_t는 8 * 8비트 => 64비트이므로, 64비트를 한꺼번에 검사해서 1인 비트가
		// 있는 지 확인함
		if( ( ( iMaxCount - i ) / 64 ) > 0 )
		{
			pqwBitmap = ( uint32_t* ) &( pbBitmap[ i / 8 ] );
			// 만약 8바이트가 모두 0이면 8바이트 모두 제외
			if( *pqwBitmap == 0 )
			{
				i += 32;
				continue;
			}
		}

		if( ( pbBitmap[ i / 8 ] & ( DYN_MEMORY_EXIST_FLAG << ( i % 8 ) ) ) != 0 )
		{
			return i;
		}
		i++;
	}
	return -1;
}

/**
 *  비트맵에 플래그를 설정
 */
static void csSetFlagInBitmap( int iBlockListIndex, int iOffset, uint8_t bFlag )
{
	uint8_t* pbBitmap;

	pbBitmap = g_DynMem.pstBitmapOfLevel[ iBlockListIndex ].pbBitmap;
	if( bFlag == DYN_MEMORY_EXIST_FLAG )
	{
		// 해당 위치에 데이터가 비어 있다면 개수 증가
		if( ( pbBitmap[ iOffset / 8 ] & ( 0x01 << ( iOffset % 8 ) ) ) == 0 )
		{
			g_DynMem.pstBitmapOfLevel[ iBlockListIndex ].qwExistBitCount++;
		}
		pbBitmap[ iOffset / 8 ] |= ( 0x01 << ( iOffset % 8 ) );
	}
	else
	{
		// 해당 위치에 데이터가 존재한다면 개수 감소
		if( ( pbBitmap[ iOffset / 8 ] & ( 0x01 << ( iOffset % 8 ) ) ) != 0 )
		{
			g_DynMem.pstBitmapOfLevel[ iBlockListIndex ].qwExistBitCount--;
		}
		pbBitmap[ iOffset / 8 ] &= ~( 0x01 << ( iOffset % 8 ) );
	}
}

/**
 *  할당 받은 메모리를 해제
 */
bool ckMemoryFreeBuddy( void* pvAddress )
{
	uint32_t qwRelativeAddress;
	int iSizeArrayOffset;
	uint32_t qwBlockSize;
	int iBlockListIndex;
	int iBitmapOffset;

	if( pvAddress == NULL )
	{
		return false;
	}

	// 넘겨 받은 어드레스를 블록 풀을 기준으로 하는 어드레스로 변환하여 할당했던
	// 블록의 크기를 검색
	qwRelativeAddress = ( ( uint32_t ) pvAddress ) - g_DynMem.qwStartAddress;
	iSizeArrayOffset = qwRelativeAddress / DYN_MEMORY_MIN_SIZE;

	// 할당되어있지 않으면 반환 안 함
	if( g_DynMem.pbAllocatedBlockListIndex[ iSizeArrayOffset ] == 0xFF )
	{
		return false;
	}

	// 할당된 블록이 속한 블록 리스트의 인덱스가 저장된 곳을 초기화하고, 할당된
	// 블록이 포함된 블록 리스트를 검색
	iBlockListIndex = ( int ) g_DynMem.pbAllocatedBlockListIndex[ iSizeArrayOffset ];
	g_DynMem.pbAllocatedBlockListIndex[ iSizeArrayOffset ] = 0xFF;
	// 버디 블록의 최소 크기를 블록 리스트 인덱스로 시프트하여 할당된 블록의 크기 계산
	qwBlockSize = DYN_MEMORY_MIN_SIZE << iBlockListIndex;

	// 블록 리스트 내의 블록 오프셋을 구해서 블록 해제
	iBitmapOffset = qwRelativeAddress / qwBlockSize;
	if( csFreeBuddyBlock( iBlockListIndex, iBitmapOffset ) == true )
	{
		g_DynMem.qwUsedSize -= qwBlockSize;
		return true;
	}

	return false;
}

/**
 *  블록 리스트의 버디 블록을 해제
 */
static bool csFreeBuddyBlock( int iBlockListIndex, int iBlockOffset )
{
	int iBuddyBlockOffset;
	int i;
	bool bFlag;
	uint32_t eflag;

	// 동기화 처리
	eflag = ckAsmGetEFlag();
	ckAsmCli();

	// 블록 리스트의 끝까지 인접한 블록을 검사하여 결합할 수 없을 때까지 반복
	for( i = iBlockListIndex ; i < g_DynMem.iMaxLevelCount ; i++ )
	{
		// 현재 블록은 존재하는 상태로 설정
		csSetFlagInBitmap( i, iBlockOffset, DYN_MEMORY_EXIST_FLAG );

		// 블록의 오프셋이 짝수(왼쪽)이면 홀수(오른쪽)을 검사하고, 홀수이면 짝수의
		// 비트맵을 검사하여 인접한 블록이 존재한다면 결합
		if( ( iBlockOffset % 2 ) == 0 )
		{
			iBuddyBlockOffset = iBlockOffset + 1;
		}
		else
		{
			iBuddyBlockOffset = iBlockOffset - 1;
		}
		bFlag = csGetFlagInBitmap( i, iBuddyBlockOffset );

		// 블록이 비어있으면 종료
		if( bFlag == DYN_MEMORY_EMPTY_FLAG )
		{
			break;
		}

		// 여기까지 왔다면 인접한 블록이 존재하므로, 블록을 결합
		// 블록을 모두 빈 것으로 만들고 상위 블록으로 이동
		csSetFlagInBitmap( i, iBuddyBlockOffset, DYN_MEMORY_EMPTY_FLAG );
		csSetFlagInBitmap( i, iBlockOffset, DYN_MEMORY_EMPTY_FLAG );

		// 상위 블록 리스트의 블록 오프셋으로 변경하고, 위의 과정을 상위 블록에서
		// 다시 반복
		iBlockOffset = iBlockOffset/ 2;
	}

	ckAsmSetEFlag(eflag);
	return true;
}

/**
 *  블록 리스트의 해당 위치에 비트맵을 반환
*/
static uint8_t csGetFlagInBitmap( int iBlockListIndex, int iOffset )
{
	uint8_t* pbBitmap;

	pbBitmap = g_DynMem.pstBitmapOfLevel[ iBlockListIndex ].pbBitmap;
	if( ( pbBitmap[ iOffset / 8 ] & ( 0x01 << ( iOffset % 8 ) ) ) != 0x00 )
	{
		return DYN_MEMORY_EXIST_FLAG;
	}

	return DYN_MEMORY_EMPTY_FLAG;
}

static bool ckMemoryCheckSizeSub(volatile uint32_t *ptr)
{
	volatile uint32_t old = *ptr;
	bool ret = false;
	*ptr = 0x1234abcd;
	*ptr ^= 0xffffffff;
	if (*ptr == ~0x1234abcdU)
	{
		*ptr ^= 0xffffffff;
		if (*ptr == 0x1234abcdU)
		{
			ret = true;
		}
	}
	*ptr = old;
	return ret;
}
uint32_t ckMemoryCheckSize(void)
{
	uint32_t cr0;

	volatile uint32_t ptr;

	cr0 = ckAsmGetCr0();
	ckAsmSetCr0(cr0 | CR0_DISABLE_CACHE);

	for (ptr = DYN_MEMORY_START_ADDRESS; ptr < IOMAP_MEMORY_START_ADDRESS; ptr += 0x100000)
	{
		if (!ckMemoryCheckSizeSub((volatile uint32_t *)ptr))
			break;
	}
	if (!ckMemoryCheckSizeSub((volatile uint32_t *)(ptr - 4)))
	{
		ptr -= 0x100000;
	}

	ckAsmSetCr0(cr0);

	g_DynMem.DynMemSize = ptr - DYN_MEMORY_START_ADDRESS;
	return g_DynMem.DynMemSize;
}


