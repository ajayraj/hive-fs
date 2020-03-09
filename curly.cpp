#include <curl/curl.h>
#include <iostream>

using namespace std;

//PORTNUM = port of node
static string NETWORK = "http://jami.bootstrap.net:4222/";

CURL *curl;
CURLcode res;

int init_node(){
  // string name = "N";
  // int i = 1;

   //do {
    //  char * temp = name + i;
      curl_easy_setopt(curl, CURLOPT_URL, NETWORK + '3');

      //GET CODE
      res = curl_easy_perform(curl);
      curl_easy_cleanup(curl);

      cout << "get result: " << res << endl;

      //Want there to be no existing value so that occupies soonest space.
    //  if (res == NULL){
    //     NODE_ID = temp;

         //POST CODE
    //     curl_easy_setopt(curl, CURLOPT_URL, NODE_ID);

         //OpenDHT may need response posted as JSON, might use complicated setup...
    //     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, '0');
    //     res = curl_easy_perform(curl);
    //     curl_easy_cleanup(curl);

    //     i = 0;
    //  }

    //  else{
    //     ++i;
    //  }

  // } while(i != 0);

   return 0;
}

int main() {
  int test = init_node();
  cout << test << endl;
}
