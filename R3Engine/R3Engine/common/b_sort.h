#ifndef __B_SORT_H__
#define __B_SORT_H__

#include <windows.h>

//라딕스 소트 or byte소트 :100개 이상이면 세상에서 가장 빠른 소팅방법....
//temp_index 와 temp_value는 sort_index 와 sort_value temp 배열이다. 
//고로 각각 사이즌 반드시 같아야한다.

// 결과를 오름차순으로 sort_index에 소팅된 인덱스들을 넣어준다. 
// %주의할점은 sort_value값은 셋팅되지 않는다. 정렬된 값들은 안쓰니까....
 

void ByteSortForShort(DWORD sort_num,DWORD *sort_index,short *sort_value
					  ,DWORD *temp_index,short *temp_value);

#endif
