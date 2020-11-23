#include<bits/stdc++.h>
#include<thread>
#include<mutex>
using namespace std;
pthread_mutex_t lock2; 
pthread_barrier_t my_barrier;
void update(vector<vector<double>> &original,int i,int j, vector<vector<double>> &duplicate)
{
    int n=original.size();
    double sum=original[i][j];
    int included=1;
    if(i-1>=0)
    {
        sum+=original[i-1][j];
        included++;
    }
    if(j-1>=0)
    {
        sum+=original[i][j-1];
        included++;
    }
    if(i+1<n)
    {
        sum+=original[i+1][j];
        included++;
    }
    if(j+1<n)
    {
        sum+=original[i][j+1];
        included++;
    }
    duplicate[i][j]=sum/included;
}
void stencil( int tid, int nt, vector<vector<double>> &original,vector<vector<double>> &duplicate, double &max_change, double threshold, int &number_of_iterations )
{
    int n=original.size(),rows_per_thread=n/nt;
    threshold=0.1;
    while(max_change >= threshold )
    {
        double max_change_in_current=0;

        if(tid==nt-1)
        {
            for(int i=rows_per_thread*tid;i<n;i++)
            {
                for(int j=0;j<n;j++)
                {
                    update(original,i,j,duplicate);
                    max_change_in_current=max( max_change_in_current, abs(original[i][j]-duplicate[i][j]) );
                }
            }
        }
        else
        {
            for(int i=rows_per_thread*tid;i<rows_per_thread*(tid+1);i++)
            {
                for(int j=0;j<n;j++)
                {
                    update(original,i,j,duplicate);
                    max_change_in_current=max( max_change_in_current, abs(original[i][j]-duplicate[i][j]) );
                }
            }
        }
        pthread_barrier_wait(&my_barrier);
        if(tid==0)
        {
            pthread_mutex_lock(&lock2);
            max_change=0;
            number_of_iterations++;
            pthread_mutex_unlock(&lock2);
        }
        pthread_barrier_wait(&my_barrier);
        if(tid==nt-1)
        {
            for(int i=rows_per_thread*tid;i<n;i++)
            {
                for(int j=0;j<n;j++)
                {
                    original[i][j]=duplicate[i][j];
                }
            }
        }
        else
        {
            for(int i=rows_per_thread*tid;i<rows_per_thread*(tid+1);i++)
            {
                for(int j=0;j<n;j++)
                {
                    original[i][j]=duplicate[i][j];
                }
            }
        }        
        pthread_mutex_lock(&lock2);
        max_change=max(max_change,max_change_in_current);
        pthread_mutex_unlock(&lock2);
        pthread_barrier_wait(&my_barrier);
    }    
}
void example_random_number_generator(vector<vector<double>> &matrix)
{
   double lbound = 0;
   double ubound = 10;
   std::uniform_real_distribution<double> urd(lbound, ubound);
   std::default_random_engine re;
   int n=matrix.size();
   for(int i=0;i<n;i++)
   {
       for(int j=0;j<n;j++)
       {
           matrix[i][j]=urd(re);
       }
   }

}
int main(int argc, char **argv)
{
    int n=atoi(argv[1]), nt=4;
    double threshold=atof(argv[2]);   

    if(n<=16)
    {
        nt=1;
    }
    else if(n<=128)
        nt=2;
    else if(n<=512)
    {
        nt=3;
    }
    else if(n<=1024)
    {
        nt=4;
    }
    else
        nt=16;

    pthread_barrier_init(&my_barrier,NULL,nt);


    double max_change=INT_MAX;
    int count=0, number_of_iterations=0;  

    vector<vector<double>> original(n,vector<double>(n,0)), duplicate(n,vector<double>(n,0));
    vector<thread> threads;

    example_random_number_generator(original);

    

    auto start = chrono::high_resolution_clock::now();
    for(int i=0;i<nt;i++)
    {
        threads.push_back( thread(stencil,i,nt,ref(original),ref(duplicate), ref(max_change),threshold, ref(number_of_iterations)) );
    }
    for(auto &th:threads)
        th.join();        

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    cout<<"number of iterations="<<number_of_iterations<<endl;
    cout<<"time taken= "<<duration.count()  <<" microseconds"<<endl;
}
