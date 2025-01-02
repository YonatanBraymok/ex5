/**********************
* Name: 
* ID: 
* Exercise: 
***********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_FIX 1
#define WATCH_PLAYLIST 1
#define ADD_PLAYLIST 2
#define REMOVE_PLAYLIST 3
#define EXIT_ALL 4
#define SHOW 1
#define ADD_SONG 2
#define DELETE_SONG 3
#define SORT 4
#define PLAY 5
#define EXIT_PLAYLIST 6
#define ALLOCATION_FAILED 1

typedef struct Song {
    char* title;
    char* artist;
    int year;
    char* lyrics;
    int streams;
} Song;

typedef struct Playlist {
    char* name;
    Song** songs;
    int songsNum;
} Playlist;

//Prints Menu
void printPlaylistsMenu();
//Scan input function
char *scanInput();
//Playlist functions
Playlist *addPlaylist(Playlist *playlists, int *plCounter);
Playlist *removePlaylist(Playlist *playlists, int *plCounter);
void freePlaylist(Playlist playlist);
void watchPlaylist(Playlist *playlist, int playlistCounter);
void sortPlaylist(Playlist *playlist);
void showPlaylist(Playlist playlist);
//Song functions
void addSong(Playlist *playlist);
void deleteSong(Playlist *playlist);
void playSong(Playlist *playlist);
void freeSong(Song *song);
//Sorting functions:
void swapSongs(Song **a, Song **b);
void sortByYear(Playlist *playlist);
void sortByStreamsA(Playlist *playlist);
void sortByStreamsB(Playlist *playlist);
void sortAlphabetically(Playlist *playlist);

int main() {
    int plCounter = 0; //counter for playlists
    Playlist *playlists = NULL;
    int choice;
    do {
        printPlaylistsMenu();
        scanf(" %d", &choice);
        switch (choice) {
            case WATCH_PLAYLIST: {
                watchPlaylist(playlists, plCounter);
                break;
            }
            case ADD_PLAYLIST: {
                playlists = addPlaylist(playlists, &plCounter);
                break;
            }
            case REMOVE_PLAYLIST: {
                playlists = removePlaylist(playlists, &plCounter);
                break;
            }
            case EXIT_ALL: {
                break;
            }
            default: {
                printf("Invalid option\n");
                break;
            }
        }
    } while (choice != EXIT_ALL);
    if (playlists != NULL) {
        for (int i = 0; i < plCounter; i++) {
            if (&playlists[i] != NULL)
                freePlaylist(playlists[i]);
        }
        free(playlists);
    }
    printf("Goodbye!\n");  
}

void printPlaylistsMenu() {
    printf("Please Choose:\n");
    printf("\t1. Watch playlists\n"
           "\t2. Add playlist\n"
           "\t3. Remove playlist\n"
           "\t4. exit\n");
}

/* the next function is used to read an input from the character and allocate the exact amount of memory needed for this
 * said input. in each allocation we do we make sure that it succeeded, else, we exit the program.
 * each time we create the necessary size of array in order to fit the size of the input */
char *scanInput() {
    int size = 0; //current size of the input
    int capacity = 1; //current size needed to fit extra char
    char *input = malloc(capacity * sizeof(char)); //allocate the minimal memory needed for incoming input
    if (input == NULL) {
        printf("Could not allocate memory for input!\n");
        exit(ALLOCATION_FAILED);
    }
    char ch;
    while (scanf("%c", &ch)  == 1 && ch != '\n') { //read input character by character until reaching '\n'
        input[size++] = ch;
        if (ch == '\r')
            input[size - 1] = '\0'; //remove whitespace. (-1 for the correct place.
        //resize allocated memory if needed for the rest of the input
        if (size >= capacity) {
            capacity++;
            input = realloc(input, capacity * sizeof(char));
            if (input == NULL) {
                printf("Could not allocate memory for input!\n");
                exit(ALLOCATION_FAILED);
            }
        }
    }
    input[size] = '\0'; //null terminate the string
    //if the input is NULL, free the allocated memory and return NULL.
    if (size == 0) {
        free(input);
        return NULL;
    }
    return input;
}

Playlist *addPlaylist(Playlist *playlists, int *plCounter) {
    printf("Enter playlist's name:\n");
    scanf("%*[^\n]"); // clear buffer before reading input
    scanf("%*c");

    Playlist newPlaylist; //create the new playlist that is going to be added
    char *tempPlaylistName = scanInput(); //scan playlist name and store it in a temporary value.
    if (tempPlaylistName == NULL) { //this means the allocation for the playlist name failed.
        printf("Invalid playlist name.\n");
        return playlists; // return unchanged playlists array
    }
    newPlaylist.name = malloc(strlen(tempPlaylistName) + 1); //allocate memory for according to the name's length
    if (newPlaylist.name == NULL) {
        printf("Could not allocate memory for playlist name.\n");
        free(tempPlaylistName);
        exit(1);
    }
    strcpy(newPlaylist.name, tempPlaylistName); //copy name from temporary value to where it's supposed to be
    free(tempPlaylistName); //free allocated memory
    playlists = realloc(playlists, (*plCounter + 1) * sizeof(Playlist)); //create extra space for new playlist
    if (playlists == NULL) {
        printf("Could not allocate memory for playlists.\n");
        exit(1);
    }
    // initiate new playlist's values to the playlists array
    playlists[*plCounter] = newPlaylist;
    playlists[*plCounter].songsNum = 0; // New playlist starts empty
    playlists[*plCounter].songs = NULL;
    (*plCounter)++;
    return playlists; // return playlists value containing new playlist.
}

Playlist *removePlaylist(Playlist *playlists, int *plCounter) {
    int choice;
    printf("Choose a playlist:\n");
    for (int i = 0; i < *plCounter; i++) { //prints all playlists
        printf("\t%d. %s\n", i + ARRAY_FIX, playlists[i].name);
    }
    printf("\t%d. Back to main menu\n", *plCounter + 1); //menu option

    scanf("%d", &choice);
    if (choice == *plCounter + 1) { //menu option
        return playlists;
    }
    if (choice < 1 || choice > *plCounter) { //invalid option
        printf("Invalid option\n");
        return playlists;
    }

    int plIndex = choice - ARRAY_FIX; //fix choice according to the index in the array.
    freePlaylist(playlists[plIndex]); //free all allocated memory associated with the playlist
    //shift all remaining playlists to fill the gap that was created by removing the playlist
    for (int i = plIndex; i < *plCounter - 1; i++) {
        playlists[i] = playlists[i + 1];
    }
    (*plCounter)--;

    playlists = realloc(playlists, (*plCounter) * sizeof(Playlist));
    if (*plCounter > 0 && playlists == NULL) {
        printf("Memory reallocation failed.\n");
        exit(1);
    }
    printf("Playlist deleted.\n");
    return playlists; //return updated playlists array
}

//free all allocated memories associated with a playlist
void freePlaylist(Playlist playlist) {
    //free all the songs in playlist
    for (int i = 0; i < playlist.songsNum; i++) {
        if (playlist.songs[i] != NULL)
            freeSong(playlist.songs[i]);
    }
    if (playlist.songs != NULL)
        free(playlist.songs); // free the array of song pointers
    free(playlist.name);  // free the playlist name
}

void watchPlaylist(Playlist *playlist, int plCounter) {
    int pl_choice, opt_choice;
    printf("Choose a playlist:\n");
    for (int i = 0; i < plCounter; i++) { //prints all playlists
        printf("\t%d. %s\n", i + ARRAY_FIX, playlist[i].name);
    }
    printf("\t%d. Back to main menu\n", plCounter + 1); //menu option
    scanf("%d", &pl_choice);
    if (pl_choice == plCounter + 1) { //menu option
        return;
    }
    if (pl_choice < 1 || pl_choice > plCounter) { //invalid option
        printf("Invalid option\n");
        return;
    }

    //print the user's choice and options menu
    printf("playlist %s:\n", playlist[pl_choice - ARRAY_FIX].name);
    do {
        printf("\t1. Show Playlist\n"
           "\t2. Add Song\n"
           "\t3. Delete Song\n"
           "\t4. Sort\n"
           "\t5. Play\n"
           "\t6. exit\n");
        scanf("%d", &opt_choice);
        if (opt_choice < 1 || opt_choice > 6) {
            printf("Invalid option\n");
            return;
        }
        switch (opt_choice) {
            case SHOW: {
                showPlaylist(playlist[pl_choice - ARRAY_FIX]);
                playSong(&playlist[pl_choice - ARRAY_FIX]);
                break;
            }
            case ADD_SONG: {
                addSong(&playlist[pl_choice - ARRAY_FIX]);
                break;
            }
            case DELETE_SONG: {
                showPlaylist(playlist[pl_choice - ARRAY_FIX]);
                deleteSong(&playlist[pl_choice - ARRAY_FIX]);
                break;
            }
            case SORT: {
                sortPlaylist(&playlist[pl_choice - ARRAY_FIX]);
                break;
            }
            case PLAY: {
                for (int i = 0; i < playlist[pl_choice - ARRAY_FIX].songsNum; i++) {
                    printf("Now playing %s:\n", playlist[pl_choice - ARRAY_FIX].songs[i]->title);
                    printf("$ %s $\n", playlist[pl_choice - ARRAY_FIX].songs[i]->lyrics);
                    printf("\n");
                    playlist[pl_choice - ARRAY_FIX].songs[i]->streams++;
                }
                break;
            }
            case EXIT_PLAYLIST: {
                break;
            }
            default: {
                printf("Invalid option\n");
                break;
            }
        }
    } while (opt_choice != EXIT_PLAYLIST);
    watchPlaylist(playlist, plCounter); //call this function again in order to show the playlists again
}

void sortPlaylist(Playlist *playlist) {
    int choice;
    printf("choose:\n"
           "1. sort by year\n"
           "2. sort by streams - ascending order\n"
           "3. sort by streams - descending order\n"
           "4. sort alphabetically\n");
    scanf("%d", &choice);
    switch (choice) {
        case 1: {
            sortByYear(playlist);
            break;
        }
        case 2: {
            sortByStreamsA(playlist);
            break;
        }
        case 3: {
            sortByStreamsB(playlist);
            break;
        }
        default: {
            sortAlphabetically(playlist);
            break;
        }
    }
    printf("sorted\n");
}

// prints list of existing playlists.
void showPlaylist(Playlist playlist) {
    for (int i = 0; i < playlist.songsNum; i++) {
        printf("%d.  Title: %s\n"
               "    Artist: %s\n"
               "    Released: %d\n"
               "    Streams: %d\n\n",
               i + 1, playlist.songs[i]->title, playlist.songs[i]->artist,
               playlist.songs[i]->year, playlist.songs[i]->streams);
    }
}

//functions below are implemented during Playlist sorting.
//swap between 2 Songs.
void swapSongs(Song **a, Song **b) {
    Song *temp = *a;
    *a = *b;
    *b = temp;
}

//sort the songs in a playlist by year of release.
void sortByYear(Playlist *playlist) {
    for (int i = 0; i < playlist->songsNum - 1; i++) {
        for (int j = 0; j < playlist->songsNum - i - 1; j++) {
            if (playlist->songs[j]->year > playlist->songs[j + 1]->year) {
                swapSongs(&playlist->songs[j], &playlist->songs[j + 1]);
            }
        }
    }
}

//sort the songs in a playlist by streams (ascending order)
void sortByStreamsA(Playlist *playlist) {
    for (int i = 0; i < playlist->songsNum - 1; i++) {
        for (int j = 0; j < playlist->songsNum - i - 1; j++) {
            if (playlist->songs[j]->streams > playlist->songs[j + 1]->streams) {
                swapSongs(&playlist->songs[j], &playlist->songs[j + 1]);
            }
        }
    }
}

//sort the songs in a playlist by streams (descending order)
void sortByStreamsB(Playlist *playlist) {
    for (int i = 0; i < playlist->songsNum - 1; i++) {
        for (int j = 0; j < playlist->songsNum - i - 1; j++) {
            if (playlist->songs[j]->streams < playlist->songs[j + 1]->streams) {
                swapSongs(&playlist->songs[j], &playlist->songs[j + 1]);
            }
        }
    }
}

//sort the songs in a playlist in alphabetical order
void sortAlphabetically(Playlist *playlist) {
    for (int i = 0; i < playlist->songsNum - 1; i++) {
        for (int j = 0; j < playlist->songsNum - i - 1; j++) {
            if (strcmp(playlist->songs[j]->title, playlist->songs[j + 1]->title) > 0) {
                swapSongs(&playlist->songs[j], &playlist->songs[j + 1]);
            }
        }
    }
}

void addSong(Playlist *playlist) {
    //new song and its values.
    Song newSong;
    char *songTitle, *songArtist, *songLyrics;
    int songYear;

    printf("Enter song's details\n");
    printf("Title:\n");
    scanf("%*[^\n]"); // clear buffer before reading input
    scanf("%*c");
    songTitle = scanInput(); //use scanInput function to read the title of the song
    if (songTitle == NULL) { //edge case: could not allocate memory for the song name
        printf("Invalid title.\n");
        return;
    }

    printf("Artist:\n");
    songArtist = scanInput();
    if (songArtist == NULL) {
        printf("Invalid artist.\n");
        free(songTitle);
        return;
    }

    printf("Year of release:\n");
    if (scanf("%d", &songYear) != 1) {
        printf("Invalid year.\n");
        free(songTitle); //song creation failed, free allocated memories.
        free(songArtist);
        scanf("%*[^\n]"); //clear buffer
        scanf("%*c");
        return;
    }
    scanf("%*[^\n]"); //clear buffer anyway after reading the year
    scanf("%*c");

    printf("Lyrics:\n");
    songLyrics = scanInput();
    if (songLyrics == NULL) {
        printf("Invalid lyrics.\n");
        free(songTitle);
        free(songArtist);
        return;
    }

    //populate newSong values
    newSong.title = songTitle;
    newSong.artist = songArtist;
    newSong.year = songYear;
    newSong.lyrics = songLyrics;
    newSong.streams = 0; //new songs => no streams

    //reallocate memory for the songs array
    playlist->songs = realloc(playlist->songs, sizeof(Song *) * (playlist->songsNum + 1));
    if (playlist->songs == NULL) {
        printf("Memory allocation error.\n");
        //free initial allocated memories
        free(songTitle);
        free(songArtist);
        free(songLyrics);
        exit(1);
    }

    //allocate memory for the new song
    playlist->songs[playlist->songsNum] = malloc(sizeof(Song));
    if (playlist->songs[playlist->songsNum] == NULL) {
        printf("Memory allocation error.\n");
        free(songTitle);
        free(songArtist);
        free(songLyrics);
        exit(1);
    }

    //copy the new song into the playlist
    *(playlist->songs[playlist->songsNum]) = newSong;

    //increment the number of songs because a new one was added
    playlist->songsNum++;
}

void deleteSong(Playlist *playlist) {
    int choice;
    // first we check if there is anything to delete.
    if (playlist == NULL || playlist->songs == NULL || playlist->songsNum <= 0) {
        printf("Playlist is empty or invalid.\n");
        return;
    }
    printf("choose a song to delete, or 0 to quit:\n");
    scanf("%d", &choice);
    if (choice == 0) { // user chose to quit
        return;
    }
    if (choice < 1 || choice > playlist->songsNum) { //invalid option
        printf("Invalid option\n");
        return;
    }
    int songIndex = choice - ARRAY_FIX; // a value that will old the fixed user choice according to the array.
    //free all the allocated memory.
    freeSong(playlist->songs[songIndex]);
    // shift all the songs that were ahead of the removed song to the left in order the fill the created gap.
    for (int i = songIndex; i < playlist->songsNum - ARRAY_FIX; i++) {
        playlist->songs[i] = playlist->songs[i + ARRAY_FIX];
    }
    playlist->songsNum--; // remove 1 song from the songs counter
    // now that there is free slot in our array, we reallocate memory for the reduced songs array
    playlist->songs = realloc(playlist->songs, playlist->songsNum * sizeof(Song *));
    // if there are still songs in our playlist and reallocation failed
    if (playlist->songsNum > 0 && playlist->songs == NULL) {
        printf("Memory reallocation failed.\n");
        exit(1);
    }
    printf("Song deleted successfully.\n");
}

//this function receives a song and free all the memory that was allocated for it.
void freeSong(Song *song) {
    if (song != NULL) {
        free(song->title);
        free(song->artist);
        free(song->lyrics);
        free(song);
    }
}

void playSong(Playlist *playlist) {
    int choice;
    printf("choose a song to play, or 0 to quit:\n");
    scanf("%d", &choice);
    if (choice == 0) { //quit option
        return;
    }
    if (choice < 1 || choice > playlist->songsNum) {
        printf("Invalid option\n");
        return;
    }
    //play the selected song, remove 1 from the choice in order to fit the choice to the song in the array
    printf("Now playing %s:\n", playlist->songs[choice - ARRAY_FIX]->title);
    printf("$ %s $\n", playlist->songs[choice - ARRAY_FIX]->lyrics);
    playlist->songs[choice - ARRAY_FIX]->streams++; //increment the number of streams of this song
    playSong(playlist);
}
