/*-----------------------------------------------------------------------------------------
程序名：stu_management.c
程序功能：
         1.鉴别root权限，设置管理员和学生权限，管理员和学生可以异步登录
         （如果加入网络模块就可以实现多用户同步登录）
         2.管理员可以设置/查询/修改学生所有账户信息，也可以增加新的管理员
         3.学生只有查询账户/课程/奖学金的权限
         4.自动生成配置文件，退出时并保存配置文件，防止信息丢失
         5.移植程序只需将可执行程序和配置文件移植即可，信息不会丢失
程序实现：采用链表加文件操作库函数结合过程控制实现
编译：gcc stu_management.c -o stu_management
编译环境：gcc 版本 4.5.1 20100924 (Red Hat 4.5.1-4) (GCC) 
作者：张峰   [mailto:frankzhang02010@gmail.com]
修改日期：2013-12-8
-----------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define N 3                //课程和奖学金种类的总数量和
 
 
typedef struct SCORE{
    char course_name[32];
    float score;
}Score;
 
typedef struct node{
    int auth;
    char usr_name[32];
    char password[18];
    int id;
    char stu_name[32];
    char major[32];
    Score s[N];
    struct node* next;
}Node;
 
Node *head,*last;
int usr_type;
char usr_name[32],password[18];
const char* f_name="usr_info.conf";
 
void load_data(),save_data(),export_data(),check_admin(),add_admin(),login();
void add_student(),search_student(),del(),modify(),modify_password(),logout();
void show_score(),search_course();
 
void title_win(),msg_win();
 
int main()
{
    int ch;
    load_data();
    title_win();
    check_admin();
     
loop:
    printf("请选择用户类型:");
    scanf("%d",&usr_type);
    if(usr_type!=1 && usr_type!=2)
    {
        msg_win("没有该用户类型，请重新输入，1表示admin，2表示student\n");
        goto loop;
    }
     
    if(usr_type==1)
    {
         title_win();
        login();
        while(1)
        {
            msg_win("User Type:Admin");
            msg_win("Add/Srch/Del/Mdfy/M_Psd/Quit/Exp/Save");
            printf(">");
            scanf("%d",&ch);
             
            switch(ch)
            {
                case 1:add_student();break;
                case 2:search_student();break;
                case 3:del();break;
                case 4:modify();break;
                case 5:modify_password();break;
                case 6:logout();break;
                case 7:export_data();break;
                case 8:save_data();break;
                case 9:add_admin();break;
                default:break;
            }
            getchar();
            msg_win("按任意键返回");
            getchar();
            system("clear");
        }
     
    }
    else
    {
         title_win();
        login();       
        while(1)
        {
            msg_win("User Type:Student");
            msg_win("ShowScore/SrchCourse/M_Pwd/Quit");
            printf(">");
            scanf("%d",&ch);
             
            switch(ch)
            {
                case 1:show_score();break;
                case 2:search_course();break;
                case 3:modify_password();break;
                case 4:logout();break;
                default:break;
            }
            getchar();
            msg_win("按任意键返回");
            getchar();
            system("clear");
        }
     
    }
    return 0;
}
 
void load_data()
{  
    FILE* fp=NULL;
        
    if(!(fp=fopen(f_name,"a")))
    {
        msg_win("Open file fail");
        return;
    }
     
    fclose(fp);
    fp=NULL;
     
    if(!(fp=fopen(f_name,"r+")))
    {
        msg_win("Open file fail");
        return;
    }
     
    Node* p;
    while(1)
    {    
        p=(Node*)malloc(sizeof(Node));
        if(!p) continue;
        if(!fread(p,sizeof(Node),1,fp))
            break;       
        if(head==NULL)
        {
            head=last=p;
            head->next=NULL;
        }
        else
        {
            last->next=p;
            last=p;
            last->next=NULL;
        } 
     } 
     fclose(fp);
     fp=NULL;     
}
 
void save_data()
{
    FILE* fp=NULL;
    if(!(fp=fopen(f_name,"w+")))
    {
        msg_win("Open file fail");
        return;
    }
     
    Node* p;
    p=head;
    while(p)
    {
        fwrite(p,sizeof(Node),1,fp);
         p=p->next;
    }
    fclose(fp);
    fp=NULL;
}
 
void export_data()
{
    FILE* fp;
    if(!(fp=fopen("export.log","w+")))
    {
        msg_win("Open file fail");
        return;
    }
     
    Node* p;
    int i;
    p=head;
    while(p)
    {
        fprintf(fp,"%-2d|%-15s|%-18s|%-15s|%-10d|%-18s|\n",p->auth,p->usr_name,p->password,p->stu_name,p->id,p->major);
        for(i=0;i<N;i++)
                fprintf(fp,"%s:%-5.1f|",p->s[i].course_name,p->s[i].score);
        fprintf(fp,"\n");
         p=p->next;
    }
    fclose(fp);
}
 
void check_admin()
{
    Node* p;
    p=head;
    while(p)
    {
        if(p->auth==1)
            return;
        p=p->next;
    }
    p=(Node*)malloc(sizeof(Node));
    p->auth=1;
    msg_win("请设置管理员账户");
    printf("用户名:");
    scanf("%s",p->usr_name);
    printf("密码:");
    scanf("%s",p->password);
     
    if(head==NULL)
    {
        head=last=p;
        head->next=NULL;
    }
    else
    {
        last->next=p;
        last=p;
        last->next=NULL;
    }       
    msg_win("管理员账户设置成功");
    sleep(1);
    system("clear");
}
 
void add_admin()
{
    Node* p;
    p=(Node*)malloc(sizeof(Node));
    p->auth=1;
    msg_win("添加管理员账户");
    printf("用户名:");
    scanf("%s",p->usr_name);
    printf("密码:");
    scanf("%s",p->password);
     
    if(head==NULL)
    {
        head=last=p;
        head->next=NULL;
    }
    else
    {
        last->next=p;
        last=p;
        last->next=NULL;
    }       
    msg_win("新管理员账户设置成功");
 
}
 
void login()
{
login: system("clear");
    msg_win("登录");
    printf("用户名:");
    scanf("%s",usr_name);
    printf("密码:");
    scanf("%s",password);
     
    Node* p;
    p=head;
    while(p)
 
    {
        if(!strcmp(p->usr_name,usr_name) && !strcmp(p->password,password) && p->auth==usr_type)
        {
            msg_win("登录成功");
            sleep(1);
            system("clear");
            return;
        }
        p=p->next;
    }
    msg_win("用户名或密码不正确,按任意键重新登录");
    getchar();getchar();
        goto login;
}
 
void add_student()
{
    Node* p=(Node*)malloc(sizeof(Node));
    if(!p)  return ;
     
    p->auth=2;
    msg_win("添加学生账户");
    printf("用户名:");
    scanf("%s",p->usr_name);
    printf("密码:");
    scanf("%s",p->password);
    printf("姓名:");
    scanf("%s",p->stu_name);
    printf("学号:");
    scanf("%d",&(p->id));
    printf("专业:");
    scanf("%s",p->major);
     
    if(head==NULL)
    {
        head=last=p;
        head->next=NULL;
    }
    else
    {
        last->next=p;
        last=p;
        last->next=NULL;
    }       
    msg_win("学生账户设置成功");
}
 
void search_student()
{
    msg_win("搜索账户信息");
    int stu_id,i;
    printf("请输入学号:");
    scanf("%d",&stu_id);
     
    Node* p;
    p=head;
    while(p)
    {
        if(p->id==stu_id)
        {
            printf("%d %s %s %s %d %s\n",p->auth,p->usr_name,p->password,p->stu_name,p->id,p->major);
            for(i=0;i<N;i++)
                printf("%s:%-5.1f",p->s[i].course_name,p->s[i].score);
            printf("\n");
            return;
        }
        p=p->next;
    }
    msg_win("系统中没有该学号");
}
 
void del()
{
    Node *p,*q;
    int stu_id;
     
    msg_win("删除账户信息");
    printf("请输入学号:");
    scanf("%d",&stu_id);
     
    p=head;
    while(p)
    {
        if(p->id==stu_id)
        {
            if(head==p)
                head=head->next;
            else
                q->next=p->next;            
             
            free(p);
            msg_win("删除成功！");
            return;
        }
        q=p;
        p=p->next;
    }
    msg_win("删除失败！系统中没有该学号");    
}
 
void modify()
{
    int stu_id,i;
     
    msg_win("修改课程成绩和奖学金");
    printf("请输入学号:");
    scanf("%d",&stu_id);
     
    Node* p;
    p=head;
    while(p)
    {
        if(p->id==stu_id)
        {
            for(i=0;i<N;i++)
                scanf("%s %f",p->s[i].course_name,&(p->s[i].score));
            return;
        }
        p=p->next;
    }
    msg_win("系统中没有该学号");    
}
 
void modify_password()
{
    char temp[18],new_password[18];
     
    msg_win("修改密码");
     
    printf("请输入旧密码:");
    scanf("%s",temp);
     
    printf("请输入新密码:");
    scanf("%s",new_password);
    printf("请确认新密码:");
    scanf("%s",password);
     
    Node* p;
    p=head;
    while(p)
    {
        if(!strcmp(usr_name,p->usr_name))
        {
            if(!strcmp(p->password,temp)&& !strcmp(new_password,password))
            {
                strcpy(password,new_password);
                strcpy(p->password,password);
                msg_win("密码修改成功");
                return;
            }
            else
            {
                msg_win("输入有误，自动退出");
                return;
            }
        }
        p=p->next;
    }
}
 
void logout()
{
    save_data();
    msg_win("成功退出");
    exit(1);
}
 
void show_score()
{
    int i;
    Node* p;
    p=head;
    msg_win("所有课程成绩和奖学金");
    while(p)
    {
        if(!strcmp(usr_name,p->usr_name))
        {
            printf("%d %s %s %s %d %s\n",p->auth,p->usr_name,p->password,p->stu_name,p->id,p->major);
            for(i=0;i<N;i++)
                printf("%s:%-5.1f",p->s[i].course_name,p->s[i].score);
            printf("\n");
            return;
        }
        p=p->next;
    }
}
 
void search_course()
{
    char course_name[32];
    int i;
    msg_win("输入要查询的课程");
    printf(">");
    scanf("%s",course_name);
     
     Node* p;
    p=head;
    while(p)
    {
     if(!strcmp(usr_name,p->usr_name))
     {  
        for(i=0;i<N;i++)
         {      
            if(!strcmp(course_name,p->s[i].course_name))
             {
               printf("%s:%-5.1f\n",p->s[i].course_name,p->s[i].score);
               return;
              }
        }
      }
        p=p->next;
    }
}
 
char str1[]="Student Information Management System";
 
void title_win()
{
     int i,M;
     
     M=strlen(str1);
    for(i=0;i<M;i++)   printf("-");
    printf("\n");
    printf("%s\n",str1);
    for(i=0;i<M;i++)   printf("-");
    printf("\n");
}
 
void msg_win(const char* str)
{
    int i,M;
     
    M=strlen(str1);
    printf("\n");
    printf("%s\n",str);
    for(i=0;i<M;i++)   printf("-");
    printf("\n");   
}