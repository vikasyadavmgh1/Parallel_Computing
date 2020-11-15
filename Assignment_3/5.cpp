#include<bits/stdc++.h>
#include<thread>
using namespace std;
void compute_sum(int tid,vector<double> &sum ,const vector<double> &data,int nthreads)
{
    int st=(data.size()/nthreads)*tid;
    int en=(data.size()/nthreads)*(tid+1);
    if(tid==nthreads-1)
    {
        en=data.size();
    }
    for(int i=st;i<en;i++)
    {
        sum[tid]+=data[i];
    }
}
int main(int argc,char **argv)
{
    int n=atoi(argv[1]),np=5;
    vector<double> data;
    vector<double> sum(np,0);
    vector<thread> threads;


    double lbound = 0, ubound = 10 , ans=0;
    std::uniform_real_distribution<double> urd(lbound, ubound);
    std::default_random_engine re;
    for(int i=0;i<n;i++)
    {
        data.push_back(urd(re));
        ans+=data[i];
    }
    auto start = chrono::high_resolution_clock::now();
    for(int i=0;i<np;i++)
    {
        threads.push_back( thread(compute_sum,i,ref(sum),cref(data), np) );
    }
    for(auto &th:threads)
        th.join();
    double tsum=0;
    for(int i=0;i<np;i++)
    {
        tsum+=sum[i];
    }
    auto stop = chrono::high_resolution_clock::now();

    cout<<tsum<<endl;
    //cout<<"without threads= "<<ans<<endl;
}
