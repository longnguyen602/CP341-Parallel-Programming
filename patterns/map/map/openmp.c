/**
 * This program takes in a hash and attempts to find a matching 8 digit pin
 */

// sprintf, printf
#include <stdio.h>

// strlen, strncmp
#include <string.h>

// exit value
#include <stdlib.h>

// md5 hash function
#include <openssl/md5.h>

// clock measurements
#include <time.h>

#include <omp.h>

// valid pin characters
const char* chars="0123456789";


// tests if a hash matches a candidate password
int test(const char* passhash, const char* passcandidate) {
    unsigned char digest[MD5_DIGEST_LENGTH]; // hash storage

    // Compute the hash
    MD5((unsigned char*)passcandidate, strlen(passcandidate), digest);

    // Convert the hash to a base 16 string
    char mdString[34];
    mdString[33]='\0';
    for(int i=0; i<16; i++) {
        sprintf(&mdString[i*2], "%02x", (unsigned char)digest[i]);
    }

    // return the value of the comparison
    return strncmp(passhash, mdString, strlen(passhash));
}

// maps a numeric PIN to a string
void genpass(long passnum, char* passbuff) {
    passbuff[8]='\0'; // enforce null termination
    int charidx; // character within array of valid characters

    // determine the "base" for the pin string
    int symcount=strlen(chars);

    // perform the conversion
    for(int i=7; i>=0; i--) {
        charidx=passnum%symcount;
        passnum=passnum/symcount;
        passbuff[i]=chars[charidx];
    }
}

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Usage: %s <password hash>\n",argv[0]);
        return 1;
    }

    char passmatch[9]; // buffer for the matched password
    char correct[9];
    long currpass=0; // current password under consideration

    // variables for the timer
    struct timespec start_time;
    struct timespec end_time;
    long msec;

    clock_gettime(CLOCK_MONOTONIC,&start_time);
    // While a match has not been found, search
    int notfound=1;
    #pragma omp parallel for

    //while(notfound) {
    //for (int i=0; notfound==1 && i < 100000000; i++){
    for (int i=0; i < 100000000; i++){
        // generate the password
        genpass(currpass,passmatch);
        //printf("found: %s\n",passmatch);
        // check for a match
        // if matched, return 0
        notfound=test(argv[1], passmatch);
        if (notfound==0){
          printf("\n%d\n", notfound);
          //correct = passmatch;
          strcpy(correct,passmatch);
          correct[8]='\0';
        }
        currpass++;
    }

    clock_gettime(CLOCK_MONOTONIC,&end_time);

    // convert the time to elapsed milliseconds
    msec = (end_time.tv_sec - start_time.tv_sec)*1000 + (end_time.tv_nsec - start_time.tv_nsec)/1000000;

    printf("found: %s in %dms\n",correct,msec);
    return EXIT_SUCCESS;
}
