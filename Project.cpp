#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include<stdlib.h>
#include<unistd.h>
using namespace std;

sem_t totalCustomerFull,totalCustomerEmpty;
sem_t sofaFull,sofaEmpty;
sem_t barberChairFull,barberChairEmpty;
sem_t totalBarberFull,totalBarberEmpty;
sem_t takingHaircutFull,takingHaircutEmpty;
sem_t payFull,payEmpty;
sem_t acceptedPaymentCustomerFull,acceptedPaymentCustomerEmpty;

queue<int>q1;
queue<int>q2;
queue<int>q3;
queue<int>q4;
queue<int>q5;
queue<int>q10;
pthread_mutex_t lock1;
pthread_mutex_t lock2;
pthread_mutex_t lock3;
pthread_mutex_t lock4;
pthread_mutex_t lock5;
pthread_mutex_t lock10;
int cnt=1;

void init_semaphore(){
	sem_init(&totalCustomerFull,0,0);
	sem_init(&totalCustomerEmpty,0,20);

    sem_init(&sofaEmpty,0,4);
	sem_init(&sofaFull,0,0);

    sem_init(&barberChairEmpty, 0, 3);
	sem_init(&barberChairFull, 0, 0);

    sem_init(&totalBarberEmpty, 0, 3);
	sem_init(&totalBarberFull, 0, 0);

    sem_init(&payEmpty, 0, 1);
	sem_init(&payFull, 0, 0);

    sem_init(&takingHaircutEmpty, 0, 3);
	sem_init(&takingHaircutFull, 0, 0);


	pthread_mutex_init(&lock1,0);
    pthread_mutex_init(&lock2, 0);
    pthread_mutex_init(&lock3, 0);
    pthread_mutex_init(&lock4, 0);
    pthread_mutex_init(&lock5, 0);
    pthread_mutex_init(&lock10, 0);
}

void *customer(void * arg){
     int customer_id=(intptr_t)arg;
	sem_wait(&totalCustomerEmpty);
	pthread_mutex_lock(&lock1);
	//sleep(1);
	q1.push(cnt);
	printf("customer %d enter the shop \n",cnt);
	cnt++;
	pthread_mutex_unlock(&lock1);
	sem_post(&totalCustomerFull);


	sem_wait(&sofaEmpty);
	pthread_mutex_lock(&lock2);
	//sleep(1);
    int sofaCustomer= q1.front();
    q1.pop();
    q2.push(sofaCustomer);
	printf("Customer %d sit on sofa\n",sofaCustomer);
    pthread_mutex_unlock(&lock2);
	sem_post(&sofaFull);

	sem_wait(&sofaFull);
    sem_wait(&barberChairEmpty);
	pthread_mutex_lock(&lock3);
	//sleep(1);
    int chairCustomer=q2.front();
	q3.push(chairCustomer);
    q2.pop();
	printf("customer %d sit in the barber chair\n",chairCustomer);
	pthread_mutex_unlock(&lock3);
	sem_post(&barberChairFull);
    sem_post(&sofaEmpty);

	sem_wait(&takingHaircutFull);
	pthread_mutex_lock(&lock10);
	int takingHaircutcustomer=q10.front();
	q10.pop();
	pthread_mutex_unlock(&lock10);
	sem_post(&takingHaircutEmpty);

	sem_wait(&payEmpty);
	pthread_mutex_lock(&lock5);
	//sleep(1);
	q5.push(takingHaircutcustomer);
	printf("customer %d pay the bill \n",takingHaircutcustomer);
	pthread_mutex_unlock(&lock5);
	sem_post(&payFull);
	sem_post(&barberChairEmpty);



    return 0;
}

void *barber(void * arg){
    sem_wait(&totalCustomerFull);
    sem_wait(&barberChairFull);
    int barber_id=(intptr_t)arg;
    sem_post(&totalBarberEmpty);
    pthread_mutex_lock(&lock4);
    int barberChairCustomer = q3.front();
    //sleep(1);
    q4.push(barber_id);
    q3.pop();
    printf("Barber %d is cutting the hair of customer %d \n",barber_id,barberChairCustomer);
    pthread_mutex_unlock(&lock4);
    sem_post(&totalBarberFull);

    sem_post(&takingHaircutEmpty);
    pthread_mutex_lock(&lock10);
    q10.push(barberChairCustomer);
    pthread_mutex_unlock(&lock10);
    sem_post(&takingHaircutFull);

    sem_wait(&totalBarberFull);
	pthread_mutex_lock(&lock4);
	int freeBarber=q4.front();
	q4.pop();
	pthread_mutex_unlock(&lock4);
	sem_post(&totalBarberEmpty);

	sem_wait(&payFull);
	pthread_mutex_lock(&lock5);
	int payCustomer=q5.front();
	q5.pop();
	printf("Barber %d accept the payment of customer %d \n",freeBarber,payCustomer);
	pthread_mutex_unlock(&lock5);
    sem_post(&payEmpty);

    //sem_wait(&totalCustomerFull);
	pthread_mutex_lock(&lock1);
	int exitCustomer=q1.front();
	q1.pop();
	printf("customer %d exit the shop \n",exitCustomer);
	pthread_mutex_unlock(&lock1);
	sem_post(&totalBarberFull);
    sem_post(&totalCustomerFull);
    return 0;
}

int main(){
	pthread_t Customer[20];
	pthread_t Barber[3];
    int a[20] = {1, 2, 3, 4, 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    int b[4] = {1,2,3,4};
    int c[3] = {1,2,3};
    int d[3] = {1,2,3};
	init_semaphore();
	for(int i=1;i<=20; i++){
          pthread_create(&Customer[i],NULL,customer,(int*) *(a+i));
        //pthread_create(&Customer[i],NULL,customer,NULL);
	}
	for(int i=1;i<=3; i++){
        pthread_create(&Barber[i],NULL,barber,(int*) *(c+i));
	}
	while(1);
	return 0;
}

