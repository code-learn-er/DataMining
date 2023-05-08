#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include<set>
#include <map>
//#include<unordered_map>
#include <time.h>
using namespace std;
double min_sub = 0.001,min_conf=0.7;
char in[100]="C:/Users/Lenovo/Documents/Ccode/retail.dat";
char out1[100]="C:/Users/Lenovo/Documents/Ccode/out1.txt";
char out2[100]="C:/Users/Lenovo/Documents/Ccode/guize1.txt";

typedef struct node
{
    int data[80];
    int size;
    node()
    {
        size=0;
    }
    bool operator<(const node &b)const
    {
        if(size==b.size)
        {
            for(int i=0;i<size;i++)
                if(data[i]!=b.data[i])
                    return data[i]<b.data[i];
            return false;
        }
        else return size<b.size;
    }
    bool operator==(const node &b)const
    {
        if(size==b.size)
        {
            for(int i=0;i<size;i++)
                if(data[i]!=b.data[i])
                    return false;
            return true;
        }
        else return false;
    }
}node;
node dataset[100000];
double n;
ifstream fin;
ofstream fout;
list<map<node, int> > l;
map<node,double> fp;
map<node, int> c;

//namespace std {
//    template <>
//        class hash<node>{
//        public :
//            size_t operator()(const node &a ) const
//            {
//                size_t ans=1;
//                for(int i=0;i<a.size;i++)
//                    ans=(ans<<1)^a.data[i];
//                return (ans<<1)^hash<int>()(a.size);
//            }
//    };
//};
//
//unordered_map<node,set<int> > loc;

map<node,set<int> > loc;
inline int intersection_len(node &a) //交集的长度
{
    set<int> t;
    node b=a;b.size--;
    node c;c.size=1;c.data[0]=a.data[a.size-1];
    set_intersection(loc[b].begin(),loc[b].end(),loc[c].begin(),loc[c].end(),inserter(t,t.begin()));
    
    loc[a]=t;
    return t.size();
}

inline int fun1(node &a, node &b) //a和b的最后一位元素不相等，其余元素都相等？
{
    if(a.size==1) return 1;
    if (a.data[a.size-1]==b.data[b.size])
        return 0;
    for (int i=0; i<a.size-1; i++)
    {
        if (a.data[i]!=b.data[i])
            return 0;
    }
    return 1;
}
inline int fun2(node &a, map<node, int> &b) //a的k-1项子集是否全在b中出现
{
    for (int i=0; i < a.size; i++)
    {
        node c;c.size=a.size-1;
        for(int j=0,k=0;j<a.size;j++)
            if(i!=j)c.data[k++]=a.data[j];
        if (!b.count(c))
            return 0;
    }
    return 1;
}

void read_data()
{
    int line=0;
    char s[1000];
    while (fin.getline(s, 1000))
    {
        node d;d.size=0;
        int l = strlen(s);
        for (int i = 0, j = 0; j < l; j++)
        {
            if (s[j] == ' ' || j == l - 1)
            {
                if(line==280)
                {
                    cout<<"";
                }
                int tt = atoi(&s[i]);
                d.data[d.size++]=tt;
                i = j + 1;
                node ttt;ttt.data[ttt.size++]=(tt);
                loc[ttt].insert(line);
                c[ttt]+=1;
            }
        }
        dataset[line]=d;
        line++;
    }
    n=line;
}
void solve()
{   
    for (map<node,int>::iterator it=c.begin();it!=c.end();)
    {
        if(it->second/n>=min_sub)
            it++;
        else
            c.erase(it++);
    }
    cout << "filter over!!!" << c.size() << endl;
    l.push_back(c);
    while (c.size() != 0)
    {
        map<node, int> t = c;
        c.clear();
        for (map<node, int>::reverse_iterator i = t.rbegin(); i != t.rend(); i++)
        {
            node k1=i->first;
            for (map<node, int>::reverse_iterator j = i; j != t.rend(); j++)
            {
                if (i == j)
                    continue;
                node k2=j->first;
                if (fun1(k1, k2))
                {
                    k2.data[k2.size++]=k1.data[k1.size-1];
                    if(fun2(k2,t))
                    {
                        int len=intersection_len(k2);
                        if(len/n>=min_sub)
                            c[k2]=len;
                    }
                }
                else
                    break;
            }
        }
        if (c.size() == 0)
            break;
        cout<<c.size()<<endl;
        l.push_back(c);
    }
}
void write_result()
{
    int cnt = 0;
    for (list<map<node, int> >::iterator it = l.begin(); it != l.end(); it++)
    {
        for (map<node, int>::iterator it1 = it->begin(); it1 != it->end(); it1++)
        {
            for (int it2 = 0; it2 < it1->first.size; it2++)
            {
                fout << it1->first.data[it2] << " ";
            }
            cnt++;
            fout << " 支持度为" << 1.0 * it1->second / n << endl;
            fp[it1->first]=1.0*it1->second/n;
        }
    }
    // cout<<"总数："<<cnt<<endl;
    fout << "总数：" << cnt << endl;
}

void split(const node &c,int n,node& a,node& b)
{
    int t[10],cnt=0;
    for(int it=0;it!=c.size;it++)
    {
        t[cnt++]=c.data[it];
    }
    for(int i=0;i<cnt;i++)
    {
        if(n&(1<<i))
            a.data[a.size++]=(t[i]);
        else
            b.data[b.size++]=(t[i]);
    }
}
void print(node &a)
{
    for(int it=0;it!=a.size;it++)
        fout<<a.data[it]<<" ";
}
void generate()
{
    for(map<node,double>::iterator it=fp.begin();it!=fp.end();it++)
    {
        int n=it->first.size;
        for(int i=1;i<(1<<n)-1;i++)
        {
            node a,b;
            split(it->first,i,a,b);
            if(fp[it->first]/fp[a]>min_conf)
            {
                print(a);
                fout<<"==>>";
                print(b);
                fout<<"  置信度为："<<fp[it->first]/fp[a]<<endl;
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
    read_data();
    cout << "success read" << endl;
    fin.close();

    // for(auto it=c.begin();it!=c.end();it++) cout<<it->first.data[0]<<" "<<it->second<<endl;
    
    clock_t start = clock();
    solve();
    cout << "success solve" << endl;
    clock_t end = clock();
    cout << "time:" << 1.0*(end - start) / CLOCKS_PER_SEC << "s" << endl;

    fout.open(out1);
    write_result();
    cout << "success write ! cnt==" <<fp.size()<< endl;
    fout.close();

    cout<<"generate regular"<<endl;
    fout.open(out2);
    generate();
    fout.close();
    system("pause");
    return 0;
}
