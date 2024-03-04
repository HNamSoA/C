#include <stdio.h>
#include <stdlib.h>

typedef struct linked_list_node_t {
    void* data;
    struct linked_list_node_t* next;
} linked_list_node_t;

void create_array_of_linked_list_ptrs(linked_list_node_t*** destination, int size) {
    *destination = (linked_list_node_t**)malloc(sizeof(linked_list_node_t*) * size);
    for (int i = 0; i < size; i++) {
        (*destination)[i] = NULL;
    }

}

void get_element_of_array_of_linked_list_ptrs(linked_list_node_t** array, int index, linked_list_node_t** destination){
    *destination = array[index];
}

void set_element_of_array_of_linked_list_ptrs(linked_list_node_t*** array, int index, linked_list_node_t** head) {
    (*array)[index] = *head;
}

void destroy_array_of_linked_list_ptrs(linked_list_node_t*** array) {
    free(*array);
    *array = NULL;

}

void resize_array_of_linked_list_ptrs(linked_list_node_t*** destination, linked_list_node_t** array, int size, int new_size){
    *destination = (linked_list_node_t**)malloc(sizeof(linked_list_node_t*) * new_size);
    for (int i = 0; i < new_size; i++) {
        (*destination)[i] = NULL;
    }
    for(int i = 0; i < size; i++){
        (*destination)[i] = array[i];
    }
    free(array);
    array = NULL;
}

void create_link_list(linked_list_node_t** destination){
    linked_list_node_t* new_node = (linked_list_node_t*)malloc(sizeof(linked_list_node_t));
    (*new_node).data = NULL;
    (*new_node).next = NULL;
    *destination = new_node;
}

void destroy_linked_list(linked_list_node_t** head) {
   linked_list_node_t* current = *head;
   linked_list_node_t* next = NULL;
   while (current != NULL) {
       next = current->next;
       free(current);
       current = next;
   }  
   *head = NULL;
}

void add_to_linked_list(linked_list_node_t** head, const void* data) {  
    linked_list_node_t* newNode = (linked_list_node_t*)malloc(sizeof(linked_list_node_t));
    (*newNode).data = (void*)data;
    (*newNode).next = NULL;
    if ((*head)->data == NULL) {
        (*head)->data  = (void*)data;
    }
    else {
        linked_list_node_t* lastNode = *head;

        while ((*lastNode).next != NULL) {
            lastNode = lastNode->next;
        }
        lastNode->next = newNode;
    }
}

void remove_from_linked_list(linked_list_node_t** destination, linked_list_node_t** head, const void* data) {
    linked_list_node_t* current = *head;
    linked_list_node_t* previous = NULL;
    while (current != NULL) {
        if ((*current).data == data) {
            if (previous == NULL) {
                *head = current->next;
            } else {
                (*previous).next = (*current).next;
            }
            free(current);
            return;
        }
        previous = current;
        current = (*current).next;
    }
}

typedef struct song_t {
    const char* name;
    float duration;
} song_t;

void create_song(song_t* destination, const char* name, float duration) {
    song_t* new_song = (song_t*)malloc(sizeof(song_t));
    (*new_song).name = name;
    (*new_song).duration = duration;
    *destination = *new_song;  
}

linked_list_node_t** array_of_playlist_ptrs = NULL;

int main(void){

    create_array_of_linked_list_ptrs(&array_of_playlist_ptrs, 5);

    for(int i = 0; i < 5; i++){
        linked_list_node_t* playlist = NULL;
        create_link_list(&playlist);
        set_element_of_array_of_linked_list_ptrs(&array_of_playlist_ptrs, i, &playlist);
    }


    resize_array_of_linked_list_ptrs(&array_of_playlist_ptrs, array_of_playlist_ptrs, 5, 10);

    for(int i = 5; i < 10; i++){
        linked_list_node_t* playlist = NULL;
        create_link_list(&playlist);
        set_element_of_array_of_linked_list_ptrs(&array_of_playlist_ptrs, i, &playlist);
    }

    char* name[] = {"Diamonds", "Roses", "Bloom", "Halo",
    "Istanbulda Sonbahar", "Bir Ayrilik Bir Yoksulluk Bir Olum", "Bir Günah Gibi", "Bir Nefes Gibi",
    "Bir Yagmur Masali", "Bir Zamanlar Boxset", "Kuzu Kuzu", "Senden Once Senden Sonra",
    "Neyse", "Son Arzum", "Ben Senden Vazgectim", "Ben Hala Vazgecmedim",
    "Zamanla Ya Da Zorla", "Bosver Be", "Neden Bu Kadar Guzelsin", "Bi Bak Bana",
    "Radioactive", "Demons", "On Top Of The World", "Bleeding Out",
    "Mockingbird", "When I'm Gone", "Lose Yourself", "Not Afraid",
    "The Real Slim Shady", "Without Me", "Stan", "The Monster",
    "The Way I Am", "Love The Way You Lie", "Beautiful", "Just Lose It",
    "Sing For The Moment", "Like Toy Soldiers", "Berzerk", "Rap God"};

    double duration[] = {3.2,4.7,2.9,1.4,4.1,2.8,3.3,4.6,1.9,3.5,4.3,2.1,1.8,4.5,2.7,3.6,1.3,4.4,3.9,1.2,4.9,2.4,3.1,1.5,4.2,2.6,3.7,1.7,3.8,1.1,4.8,2.3,3.4,1.6,4.0,2.0,3.0,1.0,5.0,2.2};
    song_t song[40];
    for(int i = 0; i < 40; i++){
        create_song(&song[i], name[i], duration[i] );
        add_to_linked_list(&array_of_playlist_ptrs[i % 10], &song[i]);
    }

    for (int i = 0; i < 10; i++) {
        linked_list_node_t* playlist = NULL;
        get_element_of_array_of_linked_list_ptrs(array_of_playlist_ptrs, i, &playlist);

        if (playlist != NULL && playlist->next != NULL) {
            linked_list_node_t* secondSongNode = playlist->next;
            song_t* secondSong = (song_t*) secondSongNode->data;
            printf("Removing second song from Playlist %d: %s\n", i + 1, secondSong->name);
            remove_from_linked_list(&array_of_playlist_ptrs[i],&playlist, secondSong);
        }

        printf("Playlist %d after removing second song:\n", i + 1);
        linked_list_node_t* current = array_of_playlist_ptrs[i];
        int songIndex = 1;

        while (current != NULL) {
            song_t* song = (song_t*) current->data;
            printf("%d. Song: %s, Duration: %.1f minutes\n", songIndex, song->name, song->duration);
            current = current->next;
            songIndex++;
        }

        destroy_linked_list(&playlist);  

        printf("\n");
    }

    destroy_array_of_linked_list_ptrs(&array_of_playlist_ptrs);

    return 0;
}
