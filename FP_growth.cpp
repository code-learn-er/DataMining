#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<fstream>
#include<list>
#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include<time.h>
using namespace std;
#define inf 0x3f3f3f3f

double min_sub=0.01,min_conf=0.7;
char in[100]="C:/Users/Lenovo/Documents/Ccode/retail.dat";
char out1[100]="C:/Users/Lenovo/Documents/Ccode/out2.txt";
char out2[100]="C:/Users/Lenovo/Documents/Ccode/guize2.txt";

typedef struct node
{
    int item;
    int cnt=0;
    map<int,node*> child;
    node* father=NULL;
    node* next=NULL;
}node,Tree;
typedef struct triple
{
    int item;
    int cnt;
    node* next;
}triple;
typedef struct FP_tree
{
    vector<triple> head;
    map<int,node**> last;
    Tree root;
}FP_tree;

list<vector<int> > dataset;
double n;
ifstream fin;
ofstream fout;
list<pair<vector<int>,double> > l;

inline void filter(map<int,int> &c)//过滤
{
    for(map<int,int>::iterator it=c.begin();it!=c.end();)
    {
        if(it->second<min_sub*n) 
            c.erase(it++);
        else 
            it++;
    }
}

void read_data()
{
    char s[1000];
    while(fin.getline(s,1000))
    {
        vector<int> d;
        int l=strlen(s);
        for(int i=0,j=0;j<l;j++)
        {
            if(s[j]==' '||j==l-1)
            {
                int tt=atoi(&s[i]);
                d.push_back(tt);
                i=j+1;
            }
        }
        dataset.push_back(d);
    }
}

void insert(FP_tree &ans,Tree &root,set<pair<int,int> >&q,int cnt)
{
    if(q.size()==0)return;
    set<pair<int,int> >::reverse_iterator it=q.rbegin();

    int item=-it->second;
    q.erase(make_pair(it->first,-item));
    if(root.child.count(item))
    {
        root.child[item]->cnt+=cnt;
        insert(ans,*root.child[item],q,cnt);
    }
    else
    {
        node* a=new node;
        root.child[item]=a;
        a->item=item;
        a->cnt=cnt;
        a->father=&root;
        *(ans.last[item])=a;
        ans.last[item]=&(a->next);
        insert(ans,*a,q,cnt);
    }
}
void buildtree(list<vector<int> >&dataset,vector<int>& cnt,FP_tree &ans)
{
    map<int,int> item;
    int i=0;
    for(list<vector<int> >::iterator it=dataset.begin();it!=dataset.end();it++,i++)//scan
    {
        for(vector<int>::iterator it1=it->begin();it1!=it->end();it1++)
        {
            item[*it1]+=cnt[i];
        }
    }
    filter(item);//过滤

    pair<int,int> t[int(item.size())];//pair<-cnt,item>
    i=0;
    for(map<int,int>::iterator it=item.begin();it!=item.end();it++)//转换为pair<-cnt,item>
    {
        t[i++]=make_pair(-it->second,it->first);
    }

    sort(t,t+item.size());//按出现次数排序
    //head<item,cnt,null>
    ans.head.reserve(int(item.size()));
    for(i=0;i<item.size();i++)//初始化head表
    {
        ans.head.push_back({t[i].second,-t[i].first,NULL});
    }
    for(int i=0;i<ans.head.size();i++)//初始化last指针
    {
        ans.last[ans.head[i].item]=&(ans.head[i].next);
    }
    i=0;
    for(list<vector<int> >::iterator a=dataset.begin();a!=dataset.end();a++,i++)//遍历dataset
    {
        set<pair<int,int> > q;// <cnt,-item>
        for(int i=0;i<a->size();i++)//对当前记录排序
        {
            if(item.count((*a)[i]))
            {
                q.insert(make_pair(item[(*a)[i]],-(*a)[i]));
            }
        }
        insert(ans,ans.root,q,cnt[i]);//插入
    }
}
bool is_chain(Tree &root)
{
    if(root.child.size()>1)return 0;
    else if(root.child.size()==0)return 1;
    else return is_chain(*root.child.begin()->second);
}
void dfs(list<vector<int> > &dataset,vector<int>&cnt,list<pair<vector<int>,double> > &ans,int k)
{
    FP_tree fp_tree;
    buildtree(dataset,cnt,fp_tree);
    if(is_chain(fp_tree.root))//如果是直链
    {
        int n=fp_tree.head.size();
        for(int i=0;i<(1<<n);i++)
        {
            vector<int> t;int minv=inf;
            for(int j=0;j<n;j++)
            {
                if(i&(1<<j))
                {
                    t.push_back(fp_tree.head[j].item);
                    minv=min(minv,fp_tree.head[j].cnt);
                }
            }
            ans.push_back(make_pair(t,minv));
        }
        return;
    }

    for(vector<triple>::reverse_iterator it=fp_tree.head.rbegin();it!=fp_tree.head.rend();it++)//循环head表，分别求解
    {
        int cur=it->item;
        list<vector<int> > subdataset;//子数据集
        vector<int> subcnt;
        for(node* i=it->next;i!=NULL;i=i->next)
        {
            vector<int> t;
            for(node* j=i->father;j->father!=NULL;j=j->father)
            {
                t.push_back(j->item);
            }
            subdataset.push_back(t);
            subcnt.push_back(i->cnt);
        }
        
        list<pair<vector<int>,double> > subans;

        dfs(subdataset,subcnt,subans,k+1);
        for(list<pair<vector<int>,double> >::iterator i=subans.begin();i!=subans.end();i++)
        {
            i->first.push_back(cur);
            if(i->first.size()==1)
                i->second=it->cnt;
        }
        ans.splice(ans.end(),subans);
    }
    if(k!=0)
    {
        vector<int> t;
        ans.push_back(make_pair(t,0));
    }
}

void solve()
{
    n=dataset.size();
    vector<int> cnt(int(n),1);
    dfs(dataset,cnt,l,0);
}
void write_result()
{
    for(list<pair<vector<int>,double> >::iterator it=l.begin();it!=l.end();it++)
    {
        sort(it->first.begin(),it->first.end());
        for(vector<int>::const_iterator it1=it->first.begin();it1!=it->first.end();it1++)
            fout<<*it1<<" ";
        fout<<" 支持度为"<<(it->second=1.0*it->second/n)<<endl;
    }
    fout<<"总数："<<l.size()<<endl;
}

void split(const vector<int> &c,int n,vector<int>& a,vector<int>& b)
{
    int t[100],cnt=0;
    for(int it=0;it!=c.size();it++)
    {
        t[cnt++]=c[it];
    }
    for(int i=0;i<cnt;i++)
    {
        if(n&(1<<i))
            a.push_back(t[i]);
        else
            b.push_back(t[i]);
    }
}
void print(vector<int> &a)
{
    for(vector<int>::iterator it=a.begin();it!=a.end();it++)
        fout<<*it<<" ";
}
void generate()
{
    map<vector<int>,double> m;
    for(list<pair<vector<int>,double> >::iterator it=l.begin();it!=l.end();it++)
    {
        m[it->first]=it->second;
    }
    // l.clear();
    for(list<pair<vector<int>,double> >::iterator it=l.begin();it!=l.end();it++)
    {
        int n=it->first.size();
        for(int i=1;i<(1<<n)-1;i++)
        {
            vector<int> a,b;
            split(it->first,i,a,b);
            if(m[it->first]/m[a]>min_conf)
            {
                print(a);
                fout<<"==>>";
                print(b);
                fout<<"  置信度为："<<m[it->first]/m[a]<<endl;
            }
        }
    }
}
int main()
{
	cin>>min_sub; 
    fin.sync_with_stdio(0);
    fin.tie(0);
    fout.tie(0);

    fin.open(in);
    read_data();cout<<"success read"<<endl;
    fin.close();

    clock_t start=clock();
    solve();cout<<"success solve"<<endl;
    clock_t end=clock();
    cout<<1.0*(end-start)/CLOCKS_PER_SEC<<"s"<<endl;
    cout<<"cnt=="<<l.size()<<endl;

    // fout.open(out1);
    // write_result();cout<<"success write"<<endl;
    // fout.close();
    
    // cout<<"generate regular"<<endl;
    // fout.open(out2);
    // generate();
    // fout.close();

    system("pause");
    return 0;
}
