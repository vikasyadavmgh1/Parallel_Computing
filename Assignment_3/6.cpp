#include<bits/stdc++.h>
using namespace std;
int main(int argc, char **argv)
{
    int n=atoi(argv[1]);
    double numbers[n], total_sum=0, l=0,u=10;
    std::uniform_real_distribution<double> urd(l, u);
    std::default_random_engine re;
    for(int i=0;i<n;i++)
    {
        numbers[i]=urd(re);
    }
    #pragma omp parallel for reduction(+: total_sum)
        for(int i=0;i<n;i++)
        {
            total_sum+=numbers[i];
        }
    cout<<total_sum<<endl;
}