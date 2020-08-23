#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define clear(s) memset(s,'\0',128)
//typedef pthread_mutex_t mutex_t;
#define mutex_t pthread_mutex_t
typedef struct shop
{
	int items;
	mutex_t door;
	int status; // 0 - busy, 1 - ready, 2 - closed, 3 - empty
} shop_t;

int all_items = 30000;
shop_t shops[5];
mutex_t printer;
int closed = 0;

void customer ();
void filler_f ();
void log_out (char *s);

int main (void)
{
	pthread_mutex_init (&printer, NULL);	
	for (int i=0; i<5; i++ )
	{
		shops[i].items = 5000;
		pthread_mutex_init (&shops[i].door, NULL);
		shops[i].status = 1;
	}
	
	pthread_t customers[3], filler;
	
	for (int i=0; i<3; i++)
		pthread_create (&customers[i], NULL, customer, NULL);
	pthread_create (&filler, NULL, filler_f, NULL);

	for (int i=0; i<3; i++)
		pthread_join (customers[i], NULL);
	return 0;
}

void customer ()
{
	pid_t me = pthread_self();
	char msg[128];
	sprintf (msg, "TID#%d: Start customer job...", me);
	log_out (msg);
	//clear (msg);

	while (closed <5)
	{
		for (int i=0; i<5; i++)
		{
			
			sprintf (msg, "TID#%d: Checking %d (closed %d) shop... ", me, i+1, closed);
			log_out (msg);
			//clear (msg);
			if (shops[i].status == 1)
			{
				if ( pthread_mutex_trylock (&shops[i].door) == 0 )
				{
					sprintf (msg, "TID#%d: Shop %d available. Start shopping", me, i+1);
					log_out (msg);
					//clear (msg);
					shops[i].status = 0;
					while (shops[i].items > 0)
					{
						int sl=0;
						shops[i].items -= 100;
						if (shops[i].items == 0 && all_items == 0)
						{
                                                        shops[i].status = 2;
                                                        closed++;
                                                        sprintf (msg, "TID#%d: Shop %d will close", me, i+1);
                                                        log_out (msg);
                                                        //clear(msg); 
                                                        break;
                                                }
	 
						
					} 
					if (shops[i].status != 2)
						shops[i].status = 3;
					pthread_mutex_unlock (&shops[i].door);
					sprintf (msg, "TID#%d: Finish shoping. Get chill B-)", me);
					log_out (msg);
					//clear (msg);
					sleep (10);
				}
			}
			sleep (1);
		}
	}	
}

void filler_f ()
{
	char msg[128];
	sprintf (msg, "Filler: Start job at thread %d", pthread_self());
	log_out (msg);
	while (closed < 5)
	{
		for (int i=0; i<5; i++)
		{
			if (shops[i].status == 3)
			{
				sprintf (msg, "Filler: Find empty shop #%d. Start filling", i+1);
				log_out (msg);
				for (int j=0; j<5000; j+=100)
				{
					all_items -= 100;
					shops[i].items += 100;
					if (all_items == 0)
					{
						sprintf (msg, "Filler: GENERAL ITEMS STORE IS EMPTY");
						log_out (msg);
						shops[i].status = 1;
						return;
					}
					if (j % 1000 == 0);
						//sleep (1);
				}
				shops[i].status = 1;
				sleep (1);
			}
		}
	}
}

void log_out (char *s)
{
	pthread_mutex_lock (&printer);
	printf ("%s\n", s);
//	clear (s);
	pthread_mutex_unlock (&printer);

}

