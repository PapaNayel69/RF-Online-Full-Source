#include <sys/stat.h>
#include <stdio.h>
#include "DDSExport.h"
#include "R3BspUtil.h"
#include "Jmalloc.h"
#include "loadase.h"

BOOL IsSameExistDDS(_MATERIAL *mat,int i)		//만약에 이전에 있던DDS와 같지 않다면..
{
	FILE *fp;
	BOOL ret=FALSE;
	LONG day=-1;
	char m_name[256],path[256],su[256],hole[256];
	char name[256];


	_INIFILE *Ini=GetIniFile();
	strcpy(m_name,Ini->In_file);
	StripPath(m_name);
	StripEXT(m_name);
	
	strcpy(name,".\\Map\\");
	strcat(name,m_name);
	strcat(name,"\\Tex\\");
	strcpy(path,name);

	sprintf(su,"%s_%d",mat[i].name,i);	//서브머터리얼 이름이 같은 경우를 위해...
	strcpy(name,path);
	strcat(name,su);
	strcat(name,".mst");
	if( (fp = fopen(name,"rt"))==NULL )
		return ret;

	while(1)
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
		if( !strcmp(hole,"org_file_day"))
		{
			fscanf(fp,"%s",&hole[0]);
			day = atoi(hole);
			struct _stat status;
			char *full_path=GetFullTexPath(mat[i].tex->mapname);
			if( _stat( full_path, &status )==0 )   // static function
			{
				if( day == status.st_mtime )
					ret = TRUE;
			}
			break;
		}
	}


	fclose(fp);
	return ret;
}


void DDSTextureExport(_MESH *p_mesh,_MATERIAL *mat,char *path)
{
	int i,j,k;
	char buf[512];
	_MATERIAL *loop_mat;
	char m_name[256];

	if( mat == NULL )
	{
	    PutR3BspInfoText("Warning.. 머터리얼이 하나도 없다.!\n");
		return;
	}
	_INIFILE *Ini=GetIniFile();
	strcpy(m_name,Ini->In_file);
	StripPath(m_name);
	StripEXT(m_name);

	typedef struct{
		char name[256];
	}_NAME_STACK;
	k=0;
	for(i=0; i<mat->all_num; i++)
	{
		if( mat[i].sbnum == 0 )
			k++;
		else
			k+=mat[i].sbnum;
	}
	_NAME_STACK *name_stack=(_NAME_STACK *)Dmalloc(sizeof(_NAME_STACK)*k);
	int name_cnt=0;

	//------ㅡ디렉토리 만들기
//	MakeDir("Texture");

	char t_name[256];
	if( path )		//지정된 패스가 있는경우....
	{
		strcpy(t_name,path);
	}
	else
	{
		strcpy(t_name,".\\Map\\");
		strcat(t_name,m_name);
		strcat(t_name,"\\Tex\\");
//		strcat(m_name,"\\");
	}

//	strcat(t_name,m_name);
//	MakeDir(t_name);
	
	for(i=0; i<mat->all_num; i++)
	{
		int loop_num;
		if( mat[i].sbnum == 0 )
		{
			loop_num = 1;
			loop_mat = &mat[i];
		}
		else
		{
			loop_num = mat[i].sbnum;
			loop_mat = mat[i].submaterial;
		}
		for(j=0; j<loop_num; j++)
		{
			if(mat[i].tex!=NULL)
			{
				char output[256];

				if( path )		//지정된 패스가 있는경우....
					strcpy(output,path);
				else
					strcpy(output,t_name);//".\\Texture\\");

				char strip[256];
				char map_name[256];
				//strcat(output,m_name);
				char *full_path=GetFullTexPath(mat[i].tex->mapname);
				strcpy(map_name,full_path);
				strcpy(strip,full_path);
				StripPath(strip);
				StripEXT(strip);
				RemoveSpace(strip);
				RemoveSpace(map_name);
				strcat(output,strip);
				strcat(output,".dds");

				if( !IsSameExistDDS(mat,i) )		//만약에 이전에 있던DDS와 같지 않다면..
				{
					BOOL is_export=FALSE;
					for(k=0; k<name_cnt; k++)	//한번 출력했으면 출력시키지않는다.
					{
						if( !strcmp(strip,name_stack[k].name))
						{
							is_export = TRUE;
						}
					}
					if( is_export == FALSE )
					{
						strcpy(name_stack[name_cnt].name,strip);
						name_cnt++;

						if(mat[i].oppa!=NULL)
						{
							char *full_path=GetFullTexPath(mat[i].oppa->mapname);
							RemoveSpace(full_path);
							sprintf(buf,"convDDS -m %s %s %s",map_name,output,full_path);
							PutR3BspInfoText(buf);
							system(buf);
						}
						else
						{
							sprintf(buf,"convDDS -m %s %s",map_name,output);
							PutR3BspInfoText(buf);
							system(buf);
						}
					}
				}
				/*
				if(mat[i].oppa!=NULL)
				{
					sprintf(buf,"convDDS %s %s %s",mat[i].tex->mapname,output,mat[i].oppa->mapname);
					system(buf);
				}
				else
				{
					sprintf(buf,"convDDS %s %s",mat[i].tex->mapname,output);
					system(buf);
				}
*/
			}
		}
	}
	Dfree(name_stack);
}
