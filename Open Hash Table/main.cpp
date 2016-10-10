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
    // Конструктор
    Node ()
    {
        next = prev = NULL;
        couple.key = couple.value = hash = -1;
    }
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
    temp->couple = couple; // Присваиваем добавляемому элементу значение

    if ( list->head != NULL ) // Если список не пуст
    {
        temp->prev = list->tail; // Предыдущий элемент по отношению к добавляемому - текущий конец списка
        list->tail->next = temp;
        list->tail = temp; // Конец списка теперь - добавляемый элемент
    }
    else // Если список пуст
    {
        list->head = list->tail = temp; // Добавляемый - начало и конец списка
    }
}

void add( Node * table, int hash) // Добавление хеша (перегрузка функции) 
{    
    int i = 0;
    while (table[i].hash != -1)
        i++;
    table[i].hash = hash;
    if (i != 0)
        table[i-1].next = &table[i];
    // Жертвуем указателем prev в столбце хешей для указания на ветку (строку)
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

void print_table( Node * table ) // Печать хеш-таблицы (столбца хешей с ссылками на элементы)
{
    Node * temp_table = table;
    Node * temp_brunch; // Указатель на начало ветки (строчки) с элементами
    cout<<"Хеш-таблица:"<<endl;
    while( temp_table != NULL )
    {
        cout<<temp_table->hash<<": ";
        temp_brunch = temp_table->prev;
        while ( temp_brunch != NULL)
        {
            cout<<"("<<temp_brunch->couple.key<<", "<<temp_brunch->couple.value<<") ";
            temp_brunch = temp_brunch->next;
        }
        cout<<endl;
        temp_table = temp_table->next;
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

struct Node * find_by_hash( Node * table, int key) // Поиск по хешу
{
    Node * hash_ptr = table;
    List brunch;
    int hash;
    hash = hash_function(key);
    hash_ptr += hash;
    brunch.head = hash_ptr->prev;
    return find_first(&brunch, key);;
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

void fill_table( Node * table, List * list )
{
        Node * temp_table;
        Node * temp_brunch;
        Node * temp_list = list->head;
        while( temp_list != NULL )
        {
            temp_list->prev = NULL; // Указатель prev потребуется для связывания пар с одинаковыми хешами
            // Таким образом останется возможность идти дальше по списку (указатель next)
            temp_table = table;
            while (temp_table != NULL )
            {
                if (temp_list->hash == temp_table->hash)
                {
                    temp_brunch = temp_table->prev;
                    if (temp_brunch == NULL)
                        temp_table->prev = temp_list; 
                    else
                        temp_brunch->prev = temp_list;
                }
                temp_table = temp_table->next;
            }
            temp_list = temp_list->next;
        }
        
        // Расставим указатели в полученной хеш-таблице логично
        Node * previous;
        temp_table = table;
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
        struct List list;
        struct Node table[12];
        struct Node * found;
        struct Couple one = {1,1};
        
        // Заполнение списка 20 случайными значениями key и value от 0 до 100
        for (int i = 0; i < 20; i++)
        {
                key = rand() % 100;
                value = rand() % 100;
                struct Couple couple = {key, value};
                add(&list, couple);
        }
        print_list(&list);
        hash_function(&list);
        
        // Заполнение массива возможными хешами (столбец хешей)
        for (int i = 0; i <= 9; i++)
        {
                add(table,i);
        }
        // Заполнение хеш-таблицы по элементам, представленным списком
        fill_table(table, &list);
        
        print_table(table);
        
        // Поиск в хеш-таблице
        found = find_by_hash(table, 44);
        if (found != NULL)
            cout << "Value by key: " << found->couple.value;
        else
            cout << "Key doesn't exist"<<endl;
        
        return 0;
}