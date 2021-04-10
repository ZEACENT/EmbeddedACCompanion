#include <wctype.h>
#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*mchars_to_wchars:多字节转宽字符
 *mbs_char：多字节字符串
 *w_char：宽字符
 */
int mchars_to_wchars(const char *mbs_char, wchar_t **w_char)
{
	int mbslen = mbstowcs(NULL, mbs_char, 0);
	if (mbslen == (size_t) -1) 
	{
		perror("mbstowcs");
		return -1;
    }
    *w_char = NULL;
/*	if (setlocale(LC_CTYPE, "zh_CN.utf8") == NULL) 
	{
               perror("setlocale");
	       return -1;
        }
*/
	*w_char = calloc(mbslen + 1, sizeof(wchar_t));
	if(*w_char == NULL )
	{
		perror("calloc\n");
		return -1;
	}
	if (mbstowcs(*w_char, mbs_char, mbslen + 1) == (size_t) -1)
	{
		perror("mbstowcs");
		free(*w_char);
		return -1;
	}
	//printf("Wide character string is: %ls (%zu characters)\n", *w_char, mbslen);
	return 0;
}
