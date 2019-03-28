#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <queue>
#include<vector>

#include "graph.h"

using namespace std;

vector<int> fin;

int comp (const void *p, const void *q)
{
    long a = *(const long *)p - *(const long *)q;
    if (a > 0) return 1;
    else return -1;
}


int main(int args, char **argv)
{
	std::cout<<"Input: ./exe beg csr weight thread_count\n";
	if(args!=5){std::cout<<"Wrong input\n"; return -1;}
	
	const char *beg_file=argv[1];
	const char *csr_file=argv[2];
	const char *weight_file=argv[3];
	int thread_count=atoi(argv[4]);
	
		
    
    typedef long file_vert_t;

	//template <file_vertex_t, file_index_t, file_weight_t
	//new_vertex_t, new_index_t, new_weight_t>
	graph<file_vert_t, long, int, file_vert_t, long, char>
	*ginst = new graph
    <file_vert_t, long, int, file_vert_t, long, char>
    (beg_file,csr_file,weight_file);
	
	for(int a=0;a<=ginst->vert_count;a++)
	{
		cout<<ginst->beg_pos[a]<<"   ";
	}
	cout<<endl;
	for(int a=0;a<ginst->edge_count;a++)
	{
		cout<<ginst->csr[a]<<"    ";
	}


    //**
    //You can implement your single threaded graph algorithm here.
    //like BFS, SSSP, PageRank and etc.
    	int curr;
	int visited[ginst->vert_count]={0};
	std::queue<int> next_queue;
	curr=0;
	visited[curr]=1;
        cout<<"visited "<<curr<<endl;
	int start_edge,end_edge;
	start_edge=ginst->beg_pos[curr];
	fin.push_back(curr);
	if(curr==ginst->vert_count-1)
	end_edge=ginst->edge_count;
	else
	end_edge=ginst->beg_pos[curr+1];
	for(int j=start_edge;j<end_edge;j++)
	{
             if(visited[ginst->csr[j]]==0)
	     {
		     
		     next_queue.push(ginst->csr[j]);
                                                              
	     }
	}
    

#pragma omp parallel num_threads(thread_count) shared(visited) shared(next_queue)
	{
		int end_edge;
		int count=0;
		int curr;
		int start=0;
		int ind=0;
		int tid=omp_get_thread_num();
		int num=omp_get_num_threads();
		for(int i=tid;i<=ginst->edge_count+19;i+=num)
		{       
		        	
			ind=0;  
                        #pragma omp critical
			{
			           
				if( visited[next_queue.front()]==0)
				{
                                curr=next_queue.front();
				visited[curr]=1;
			        ind=1;	
				}
					
				if(!next_queue.empty())
				next_queue.pop();
			}
										
		        if(ind==1)
			{
			
			std::cout<<"visited "<<curr<<endl;
			int start_edge=ginst->beg_pos[curr];
			if(curr==ginst->vert_count-1)
				end_edge=ginst->edge_count;
			else
				end_edge=ginst->beg_pos[curr+1];
			if(start_edge<end_edge)
				fin.push_back(curr);
			for(int j=start_edge;j<end_edge;j++)
					{
                                               #pragma omp critical				
				                 	{
								
 						        if(visited[ginst->csr[j]]==0)
					         	{
						           next_queue.push(ginst->csr[j]);                                                      
											
							}
							
					        	}
	                         	}

			}
         	}

       }

    cout<<"parents are"<<endl;
	for(int k=0;k<fin.size();k++)
		cout<<fin.at(k)<<"   ";

std::cout<<"Start sorting the graph ... \n";
#pragma omp parallel num_threads (thread_count)
    {
        long progress = 1;
        long step = (ginst->vert_count - 1)/thread_count + 1;
        int tid = omp_get_thread_num();

        long range_beg = tid * step;
        long range_end = (tid + 1)* step;

        if(tid == thread_count - 1) range_end = ginst->vert_count;


        while(range_beg < range_end)
        {
            
            if (range_beg > progress && tid == 0)
            {
                std::cout<<range_beg * 100.0/range_end<<"% done!\n";
                progress <<=1;
            }

            int beg = ginst->beg_pos[range_beg];
            int end = ginst->beg_pos[range_beg + 1];

            qsort(ginst->csr + beg, end - beg, sizeof(file_vert_t), &comp);
            range_beg ++;

            //std::cout << i << "'s neighor list: ";
            //for (int j = beg; j < end; j++)
            //    std::cout << ginst->csr[j] << " ";
            //std::cout << "\n";
        }


    } 
        char filename[256];
        sprintf(filename, "%s_sorted.bin", argv[2]);

        FILE *fid = fopen(filename, "wb");
        assert(fid != NULL);

        assert(fwrite(ginst->csr, sizeof(file_vert_t), ginst->edge_count, fid) == ginst->edge_count);
        fclose(fid);
        
        std::cout<<"Check the "<<filename<<" for the sorted binary CSR\n";
	
	
    return 0;	
}
