#include <conio.h>
#include <stdio.h>
#include "loadase.h"
#include "R3BspUtil.h"
#include "JError.h"

void SkipToObjectStart(FILE *fp)
{
	char hole[256];
    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"[GeoObjectBegin]"))
			break;
	}
}

void OutputPreScript(char *name, _MESH *mesh,_HELPER *helper)
{
	FILE *fp1;
	FILE *fp2;
	char save_name[256],hole[512];
	int i;


	BOOL is_new=FALSE;

	strcpy(save_name,name);
	fp2 = fopen(name,"rt");
	if( fp2 == NULL )
		is_new=TRUE;
	else
	{
		fclose(fp2);
		StripEXT(save_name);
		strcat(save_name,".tmp");
	}

	if( is_new )	//스크립트 새로 작성
	{
		fp2 = fopen(save_name,"wt");	//세이브할 이름..
		if( fp2 == NULL)
		{
			Error("Script 디렉토리를 만들어놓으세요...","");
		}
		fprintf(fp2,";-ani 쓰고 끝 프래임수를 적을 것.\n");
		fprintf(fp2,";-no_merge는 엔진에서 알파 매핑 애니메이션을 쓸경우 오브젝트에 넣어준다.\n");
		fprintf(fp2,";예)  *stone_ani01 -ani 15  <- 0번부터 15번까지 애니메이션이 있다.\n");
//		fprintf(fp2,";helper는 나중에 화면전환에 쓰인다.\n");
		fprintf(fp2,"\n\n");
		fprintf(fp2,"script_begin\n\n\n");		
		fprintf(fp2,"[GeoObjectBegin]\n");
		for( i=0;i<mesh->objectnum;i++)
		{
			fprintf(fp2,"	*%s\n",mesh->object[i].objectname);
		}
		fprintf(fp2,"[GeoObjectEnd]\n\n");
		fprintf(fp2,"script_end\n");		
		fclose(fp2);
	}
	else		//기존의 스크립트에 재작업.
	{

		for( i=0;i<mesh->objectnum;i++)
		{
			fp1=fopen(name,"rt");
			BOOL ok=FALSE;
			SkipToObjectStart(fp1);
			while(1)
			{
				if( fscanf(fp1,"%s",&hole[0])==EOF)
					break;
				if( hole[0] == '*' )
				{
					if (!strcmp(&hole[1],mesh->object[i].objectname))
					{
						ok=TRUE;
						break;
					}
				}
			}
			fclose(fp1);
			if(ok!=TRUE)	//새로운 오브젝트군 추가하자...
			{
				fp1=fopen(name,"rb");
				fp2=fopen(save_name,"wb");
				while(1)
				{
					fread(&hole[0],1,1,fp1);
					if(hole[0]=='[')
					{
						fread(&hole[1],17,1,fp1);
						fwrite(&hole[0],18,1,fp2);
						break;
					}
					else
						fwrite(&hole[0],1,1,fp2);
				}
				fprintf(fp2,"	*%s\n",mesh->object[i].objectname);
				while(1)
				{
					fread(&hole[0],1,1,fp1);
					if(hole[0]=='s')
					{
						fread(&hole[1],9,1,fp1);
						hole[10]=NULL;
						fwrite(&hole[0],10,1,fp2);
						if( !strcmp(hole,"script_end"))
						{
							hole[0]=0xa;
							fwrite(hole,1,1,fp2);
							break;
						}
					}
					else
						fwrite(&hole[0],1,1,fp2);
				}
				fclose(fp1);
				fclose(fp2);
				sprintf(hole,"del %s",name);
				system(hole);
				sprintf(hole,"copy %s %s",save_name,name);
				system(hole);
				sprintf(hole,"del %s",save_name);
				system(hole);
			}
		}
	}
}

void main(void)
{
//-------------------- Ase를 파일창에서 불러온다.
	static OPENFILENAME ofn; // zero the ofn members out
    char ase_name[256]="1.map";

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = ase_name;
	ofn.nMaxFile = sizeof(ase_name);
	ofn.lpstrFilter = "R3MAP Map (*.MAP)\0*.MAP\0All (*.*)\0*.*\0";
	ofn.lpstrTitle = "Select MAP file";
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY |
               OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (!GetOpenFileName(&ofn))
		return;
//-------------------------------------------------

	printf("Reading....\n");
#ifdef 	_DEBUG
	No_warning=1;	//워닝 끄자...-.-
#endif
    _SCENE *scene=LoadSceneASE(ase_name);
    _MESH *load_mesh = LoadMeshASE(ase_name);
	if(load_mesh==NULL)
    {
        puts("object not found!");
        exit(0);
    }
    _HELPER *helper=LoadHelperASE(ase_name);		// 더미 로드

	char name[256]=".\\개발관련\\Script\\Map\\";
	StripEXT(ase_name);
	StripPath(ase_name);
	strcat(ase_name,".pst");	//pre스크립트 확장자..
	strcat(name,ase_name);
	OutputPreScript(name,load_mesh,helper);
	printf("Complete!\n");
	printf("Press any key to continue\n");
	getch();
}
