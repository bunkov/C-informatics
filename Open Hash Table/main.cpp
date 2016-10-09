#include <stdlib.h>
#include <iostream>

using namespace std;

/* Методы и классы, связанные со структурой данных "Список" следовало засунуть в эту структуру
 * Однако работа начата до знакомства с ООП в C++
 * Придется много переделывать
 */

struct Couple // Пара ключ-значение
{
    int key, value;
};

struct Node // Элемент списка
{
    struct Couple couple;
    Node *next, *prev; // Указатели на адреса следующего и предыдущего элементов списка
    int hash; // Ассоциированное с ключом число
};


struct List // Структура данных "Двусвязный список"
{
    Node *head = NULL;
    Node *tail = NULL;  
    // Указатели на адреса начала и конца списка
};


void add( List *list, struct Couple couple) // Добавление пары ключ-значение
{
    Node *temp = new Node; // Выделение памяти под новый элемент списка
    temp->next = NULL; // По умолчанию следующего элемента нет
    temp->couple = couple; // Присваиваем добавляемому элементу значение

    if ( list->head != NULL ) // Если список не пуст
    {
            temp->prev = list->tail; // Предыдущий элемент по отношению к добавляемому - текущий конец списка
            list->tail->next = temp;
            list->tail = temp; // Конец списка теперь - добавляемый элемент
    }
    else // Если список пуст
    {
            temp->prev = NULL; // Предыдущего по отношению к добавляемому нет
            list->head = list->tail = temp; // Добавляемый - начало и конец списка
    }
}

void add( List *list, int number) // Добавление пустой пары с хешом number (перегрузка функции) 
{    
    Node *temp = new Node;
    temp->next = NULL;
    temp->hash = number;
    temp->couple.key = NULL;
    temp->couple.value = NULL;
    

    if ( list->head != NULL )
    {
            temp->prev = list->tail;
            list->tail->next = temp;
            list->tail = temp;
    }
    else
    {
            temp->prev = NULL;
            list->head = list->tail = temp;
    }
}
//void pop( List *list, struct Couple couple, struct Couple *pos = NULL )

void print_list( List * list ) // Печать списка
{
        Node * temp = list->head;  // Временно указываем на адрес первого элемента
        cout<<"Список:"<<endl;
        while( temp != NULL )      // Пока не дойдем до конца списка
        {
                cout << "(" << (temp->couple).key << "," <<(temp->couple).value <<") "; // Выводим элемент
                temp = temp->next;     //Смена адреса на адрес следующего элемента
        }
        cout<<endl;
}

void print_table( List * table ) // Печать хеш-таблицы (столбца хешей с ссылками на элементы)
{    
    Node * temp = table->head;
    Node * temp_brunch; // Указатель на начало ветки (строчки) с элементами
    cout<<"Хеш-таблица:"<<endl;
    while( temp != NULL )
    {
            cout<<temp->hash<<": ";
            temp_brunch = temp->prev;
            while ( temp_brunch != NULL)
            {
                    cout<<"("<<temp_brunch->couple.key<<", "<<temp_brunch->couple.value<<") ";
                    temp_brunch = temp_brunch->next;
            }
            cout<<endl;
            temp = temp->next;
    }
    cout<<endl;
}

void hash_function( List * list ) // Сопоставляет каждому ключу одно число
{
    int hash, key;
    Node * temp = list->head;
    while( temp != NULL )
    {
            key = temp->couple.key;
            hash = key % 10;
            temp->hash = hash;
            temp = temp->next;
    }
}

int hash_function ( int key ) // Сопоставляет конкретному ключу одно число (перегрузка функции)
{
    return key % 10;
}

struct Node * find_first( List *list, int key) // Поиск по ключу с начала списка
{
    Node * temp = list->head;
    while ( temp != NULL )
    {
            if (temp->couple.key == key)
                    break;
            temp = temp->next;
    }
    return temp;
}

struct Node * find_by_hash( List *table, int key) // Поиск по хешу с начала столбца хешей
// Реализация поиска с конца столбца невозможна, т.к. столбец - односвязный список с ответвлениями
{
    Node * hash_ptr = table->head;
    Node * result;
    List * brunch;
    int hash;
    hash = hash_function(key);
    cout<<hash_ptr->hash<<" "<<hash<<endl;
    hash_ptr += hash*sizeof(Node);
    cout<<hash_ptr->hash<<endl;
    brunch->head = hash_ptr->prev; 
    return find_first(brunch, key);;
}

struct Node * find_last( List *list, int key) // Поиск по ключу с конца списка
{
    Node * temp = list->tail;
    while ( temp != NULL )
    {
            if (temp->couple.key == key)
                    break;
            temp = temp->prev;
    }
    return temp;
}

void fill_table( List * table, List * list )
{
        Node * temp_table;
        Node * temp_brunch;
        Node * temp_list = list->head;
        while( temp_list != NULL )
        {
            temp_list->prev = NULL; // Указатель prev потребуется для связывания пар с одинаковыми хешами
            // Таким образом останется возможность идти дальше по списку (указатель next)
            temp_table = table->head;
            while (temp_table != NULL )
            {
                if (temp_list->hash == temp_table->hash)
                {
                    temp_brunch = temp_table->prev;
                    if (temp_brunch == NULL or (temp_brunch->couple.key == NULL and temp_brunch->couple.value == NULL))
                        temp_table->prev = temp_list; // Жертвуем связью в столбце хешей для указания на ветку (строку) 
                    else
                        temp_brunch->prev = temp_list;
                }
                temp_table = temp_table->next;
            }
            temp_list = temp_list->next;
        }
        
        // Расставим указатели в полученной хеш-таблице логично
        Node * previous;
        temp_table = table->head;
        while( temp_table != NULL )
        {
            temp_brunch = temp_table->prev;
            // Выполним следующее только для первого элемента в ветке
            temp_brunch->next = temp_brunch->prev; // Указатель next и prev теперь совпадают
            temp_brunch->prev = NULL;
            previous = temp_brunch; // Сохраняем указатель на текущиий элемент
            temp_brunch = temp_brunch->next; // Переходим к следующему
            // Для остальных
            while ( temp_brunch != NULL)
            {
                temp_brunch->next = temp_brunch->prev;
                temp_brunch->prev = previous;
                previous = temp_brunch;
                temp_brunch = temp_brunch->next;
            }
            temp_table = temp_table->next;
        }
        /* Теперь список элементов, по которому составлялась хеш-таблица, утерян 
         * Можно копировать список вместо того, чтобы ссылаться на него,
         * однако это увеличит объем потребляемой памяти
         */
}

int main()
{
        int key, value;
        struct List list, table;
        struct Couple one = {1,1};
        for (int i = 0; i < 20; i++)
        {
                key = rand() % 100;
                value = rand() % 100;
                struct Couple couple = {key, value};
                add(&list, couple);
        }
        print_list(&list);
        hash_function(&list);
        for (int i = 0; i <= 9; i++)
        {
                add(&table,i);
        }
        fill_table(&table, &list);
        print_table(&table);
        cout<<find_by_hash(&table, 11);
        return 0;
}