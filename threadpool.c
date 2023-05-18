#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct work{
	void * (*process)(void * arg);
	void * args;
	struct work * next;
}worker;

typedef struct{
	pthread_mutex_t pool_lock;
	pthread_cond_t cond;

	worker * queuehead;
	int cur_queue_cont;
	pthread_t * pthread_num;
	int max_thread;
	int shutdown;
}pool;

pool *mypool;

void * thread_routine(void *args)
{
	pthread_t *tid = (pthread_t *)args;
	printf("start thread id = %ld.\n",*tid);
	while(1){
		pthread_mutex_lock(&mypool->pool_lock);
		if(mypool->cur_queue_cont == 0 && !mypool->shutdown){
			pthread_cond_wait(&mypool->cond,&mypool->pool_lock);
		}
		if(mypool->shutdown){
			pthread_mutex_unlock(&mypool->pool_lock);
			pthread_exit(NULL);
		}

		mypool->cur_queue_cont --;
		worker * work = mypool->queuehead;
		mypool->queuehead = work->next;
		pthread_mutex_unlock(&mypool->pool_lock);
		
		work->process((void *)work->args);

	}

}
void * invok_process(void * arg)
{
	sleep(2);
	printf("this is thread invok %d affair.\n",*(int *)arg);
}

void create_poll(int num)
{
	int i,ret;
	mypool = (pool *)malloc(sizeof(*mypool));
	if(mypool == NULL){
		printf("malloc pool memory is fail.\n");
		return ;
	}
	pthread_mutex_init(&mypool->pool_lock,NULL);
	pthread_cond_init(&mypool->cond,NULL);

	mypool->queuehead = NULL;
	mypool->shutdown = 0;
	mypool->max_thread = num;

	mypool->pthread_num = (pthread_t *)malloc(num*sizeof(pthread_t));

	for(i=0; i<num; i++)
	{
		ret = pthread_create(&(mypool->pthread_num[i]),NULL,thread_routine,(void *)&(mypool->pthread_num[i]));
		if(ret != 0)
		{
			printf("create thread is fail.\n");
		}
	}

}

void add_work(void * (*process)(void *),void *args)
{
	worker *work = (worker *)malloc(sizeof(worker));
	work->process = process;
	work->args = args;
	work->next = NULL;
	pthread_mutex_lock(&mypool->pool_lock);
	worker * pwork = mypool->queuehead;
	if(pwork != NULL){
		while(pwork->next != NULL)
			pwork = pwork->next;
		pwork->next = work;
	}else{
		mypool->queuehead = work;
	}
	mypool->cur_queue_cont ++;
	pthread_mutex_unlock(&mypool->pool_lock);
	pthread_cond_signal(&mypool->cond);

}

void destory_pool()
{
	int i;

	mypool->shutdown = 1;
	
	pthread_cond_broadcast(&mypool->cond);

	for(i=0; i<mypool->max_thread; i++){
		pthread_join(mypool->pthread_num[i],NULL);
	}
	free(mypool->pthread_num);
	
	worker * tmp;
	while(mypool->queuehead){
		tmp = mypool->queuehead;
		mypool->queuehead = mypool->queuehead->next;
		free(tmp);
	}
	pthread_mutex_destroy(&mypool->pool_lock);
	pthread_cond_destroy(&mypool->cond);
}

int main(int argc, const char *argv[])
{
	int i,num[10];
	//create thread poll
	create_poll(5);

	sleep(2);

	//add work
	for(i=0; i<10; i++){
		num[i] = i;
		add_work(invok_process,(void *)&num[i]);
	}

	sleep(3);
	//destory
	destory_pool();
	
	free(mypool);
	printf("free resource is ok.\n");
	return 0;
}
