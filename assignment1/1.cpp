#include<bits/stdc++.h>
#include "mpi.h"
using namespace std;
int find_answer_without_parallel(vector<vector<int>> a,int n,int s,int d)
{
    vector<vector<int>> ak=a;
    int length=1;
    while (ak[s][d]==0&& length<=n)
    {
        vector<vector<int>> s(n,vector<int>(n,0));
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++)
            {
                for(int k=0;k<n;k++)
                {
                    s[i][j]+=ak[i][k]*a[k][j];
                }
            }
        }
        ak=s;
        length++;
    }
    return length;
}
int main(int argc,char **argv)
{
    srand(time(0));
    int rank,number_of_process, number_of_vertex=8,&n=number_of_vertex, source=0,destination=rand()%n;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&number_of_process);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int a[n][n];
    if(source==destination)
    {
        if(rank==0)
        {
            cout<<"answer ="<<0<<endl;
            cout<<"time ="<<0<<endl;
        }
        MPI_Finalize();
        return 0;
    }
    if(rank==0)
    {
        for(int i=0;i<n;i++)
        {
            for(int j=i+1;j<n;j++)
            {
                a[i][j]=rand()%2;
                a[j][i]=a[i][j];
            }
            a[i][i]=0;
        }
    }

    int row_per_process=number_of_vertex/number_of_process;
    MPI_Bcast(&a[0][0],64,MPI_INT,0,MPI_COMM_WORLD);

    int answer_without_parallel=0;
    if(rank==0)
    {
        vector<vector<int>> graph_in_vector(n,vector<int>(n));
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++)
                graph_in_vector[i][j]=a[i][j];
        }
        answer_without_parallel=find_answer_without_parallel(graph_in_vector,n,source,destination);
        cout<<"source="<<source<<endl;
        cout<<"destination="<<destination<<endl;
        if(answer_without_parallel>=number_of_vertex)
            cout<<"no path available"<<endl;
        else
            cout<<"anwer without mpi="<<answer_without_parallel<<endl;
    }

    if(rank==0)
    {
        ofstream myfile;
        string filename="rank"+to_string(rank)+".txt";
        myfile.open(filename);
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++)
            {
                myfile<<a[i][j]<<"  ";
            }
            myfile<<endl;
        }
    }
    if(rank==0)
    {
        int at[n][n];
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++)
            {
                at[i][j]=a[i][j];
            }
        }
        int length_already=1;
        while( at[source][destination]==0 && length_already<=n )
        {
            for(int i=1;i<number_of_process;i++)
            {
                int t=1;
                MPI_Send(&t,1,MPI_INT,i,1,MPI_COMM_WORLD);
                MPI_Send(&at[i*row_per_process][0],number_of_vertex*row_per_process,MPI_INT,i,1,MPI_COMM_WORLD);
            }
            for(int i=0;i<row_per_process;i++)
            {
                vector<int> temp(number_of_vertex,0);
                for(int j=0;j<number_of_vertex;j++)
                {
                    for(int k=0;k<number_of_vertex;k++)
                    {
                        temp[j]+=at[i][k]*a[k][j];
                    }
                }
                for(int j=0;j<n;j++)
                {
                    at[i][j]=temp[j];
                }
            }
            for(int i=1;i<number_of_process;i++)
            {
                MPI_Recv(&at[i*row_per_process][0],number_of_vertex*row_per_process,MPI_INT,i,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
            length_already++;
        }
        for(int i=1;i<number_of_process;i++)
        {
            int t=0;
            MPI_Send(&t,1,MPI_INT,i,1,MPI_COMM_WORLD);

        }
        if(length_already>=n)
            cout<<"answer with mpi= no path available"<<endl;
        else
            cout<<"answer with mpi="<<length_already<<endl;
    }
    else
    {
        int cont=1;
        while(cont)
        {
            MPI_Recv(&cont,1,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            if(cont==0)
            {
                break;
            }
            int temp[row_per_process][number_of_vertex]={0};
            MPI_Recv(&temp[0][0],row_per_process*number_of_vertex,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            for(int i=0;i<row_per_process;i++)
            {
                vector<int> temp2(number_of_vertex,0);
                for(int j=0;j<number_of_vertex;j++)
                {
                    for(int k=0;k<number_of_vertex;k++)
                    {
                        temp2[j]+=temp[i][j]*a[k][j];
                    }
                }
                for(int j=0;j<n;j++)
                {
                    temp[i][j]=temp2[j];
                }
            }
            MPI_Send(&temp[0][0],row_per_process*number_of_vertex,MPI_INT,0,1,MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();

}
