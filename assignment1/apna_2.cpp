#include<bits/stdc++.h>
#include "mpi.h"
using namespace std;
int main(int argc,char **argv)
{
    srand(time(0));
    int rank,np=2, n=4, rows_per_process=n/np, s=0, d=rand()%n;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&np);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int a[rows_per_process][n];
    for(int i=0;i<rows_per_process;i++)
    {
        for(int j=0;j<n;j++)
            a[i][j]=0;
    }
    int base=rank*rows_per_process;
    for(int i=base;i<base+rows_per_process;i++)
    {
        for(int j=0;j<i;j++)
        {
            MPI_Recv( &a[i%rows_per_process][j], 1, MPI_INT, j/rows_per_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
        }
        for(int j=i+1;j<n;j++)
        {
            a[i%rows_per_process][j]=rand()%2;
            MPI_Send( &a[i%rows_per_process][j], 1, MPI_INT, j/rows_per_process, 0, MPI_COMM_WORLD);
        }
    }
    for(int i=0;i<n;i++)
    {
        if(i/rows_per_process==rank)
        {
            for(int j=0;j<n;j++)
            {
                cout<<a[i%rows_per_process][j]<<"  ";
            }
            cout<<endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    int answer_row[n]={0};
    if( rank==s/rows_per_process )
    {
        for(int i=0;i<n;i++)
        {
            answer_row[i]=a[s][i];
        }
    }
    MPI_Bcast(&answer_row, n, MPI_INT, s/rows_per_process,MPI_COMM_WORLD );
    int length_already=1;
    if(rank==0)
    {   
        while( answer_row[d]==0 && length_already<n )
        {
            for(int i=1;i<np;i++)
            {
                int t1=1;
                MPI_Send(&t1,1, MPI_INT,i,0,MPI_COMM_WORLD);
                MPI_Send(&answer_row[0], n, MPI_INT, i, 0, MPI_COMM_WORLD );
            }
            int answer_temp[rows_per_process]={0};
            for(int i=0;i<rows_per_process;i++)
            {
                for(int j=0;j<n;j++)
                {
                    answer_temp[i]+=answer_row[j]*a[i][j];
                }
            }
            for(int i=0;i<rows_per_process;i++)
            {
                answer_row[i]=answer_temp[i];
            }
            for(int i=1;i<np;i++)
            {
                MPI_Recv(&answer_row[rows_per_process*i], rows_per_process, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            length_already++;
        }
        for(int i=1;i<np;i++)
        {
            int t=0;
            MPI_Send(&t,1,MPI_INT,i,0,MPI_COMM_WORLD);

        }        
        cout<<"s= "<<s<<" d="<<d<<endl;
        if(length_already>=n)
        {
            cout<<"no possible path"<<endl;
        }
        cout<<"ans= "<<length_already<<endl;
    }
    else 
    {
        int cont=1;
        while(cont)
        {
            MPI_Recv(&cont, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(!cont)
                break;
            MPI_Recv(&answer_row, n, MPI_INT, 0 ,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int answer_temp[rows_per_process]={0};
            for(int i=0;i<rows_per_process;i++)
            {
                for(int j=0;j<n;j++)
                {
                    answer_temp[i]+=answer_row[j]*a[i][j];
                }
            }
            MPI_Send(&answer_temp[0], rows_per_process, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
}