#include <stdio.h>
#include <stdlib.h>

/*_______________________________________________________________________________________________________________________________________________________________*/
/*Краткое описание*/

/*О СЛОЖНОСТИ ПО ВРЕМЕНИ*/
/*Пусть ищем пути минимальной длины от X к Y*/
/*Чтобы алгоритм был линеен по времени, допустимы два сценария обхода графа (пока считаем, что путь есть):*/
/*1. Особая вершина не встретилась. Прошли от X к Y*/
/*2. Особая вершина встретилась. От неё нужно пройти только по оптимальным путям. В противном случае этот перебор может стать квадратичным или хуже*/
/*Для решения проблемы 2 сделаем следующее:*/
/*От конечной вершины по всем предкам начинаем увеличивать стоимости. Чтобы не допустить превышение линейной*/
/*сложности, будем отмечать посещённые вершины и больше в них стоимость не менять.*/
/*На самом деле, тогда стоимости возле вершин, которые находятся "около" X, могут оказаться*/
/*не совсем правильными (более длинный путь закроет возможность посещения для более короткого, но идущего позже).*/
/*Но это не важно, стоимости нам нужны для двух целей, не зависящих от этого: */
/*1. Стоимости всех вершин-потомков особой вершины, очевидно, будут вычислены верно. Их используем для поиска оптимальных путей*/
/*2. Если стоимость X была числом, большим или равным числу вершин графа и не уменьшилась, то X и Y лежат в разных компонентах*/
/*связности графа, а значит пути из X в Y не существует. Верно и обратное.*/
/*Из всего вышесказанного следует алгоритм*/
/*Чтобы найти еще и пути из Y в X, линейный по времени алгоритм допустимо запустить второй раз*/

/*О СЛОЖНОСТИ ПО ПАМЯТИ*/
/*struct link. Структура, линейная по занимаемой памяти. Число таких структур постоянно и не зависит от n*/

/*struct graph. Пусть в графе n элементов. Максимальная сумма чисел всех потомков всех вершин равна 2n-1*/
/*То же можно сказать и о сумме чисел предков. Для подсчёта сложности примем, что на каждый элемент структуры графа приходится не более*/
/*(2n-1)//n + 1 потомков и столько же предков. (2n-1)//n < 2. Поэтому затраты по памяти структуры graph не*/
/*превосходят О((3+3)*n) = O(6n) в грубой оценке.*/

/*struct ans_list. Структура, линейная по занимаемой памяти. Число таких структур постоянно (2) и не зависит от n*/

/*Отсюда следует линейность затрат динамической памяти*/
/*_______________________________________________________________________________________________________________________________________________________________*/


/*Произвольная цепочка вершин (используется для разных целей)*/
struct link
{
    struct graph *node;
    struct link *next;
};

/*Собственно граф (вершина)*/
struct graph
{
    int node_number;
    int cost;
    int is_special;
    int visited;
    struct link *linked_to;
    struct link *backpropagation;
    struct graph *next;
};

/*Список ответов итерации*/
struct ans_list
{
    int ans;
    struct ans_list *next;
};

/*Граф как однонаправленный список с олями связей между вершинами*/
struct graph *init_graph(int number_of_nodes)
{
    struct graph *graph, *new_node;
    int i;
    new_node = malloc(sizeof(struct graph));
    new_node->node_number = 1;
    new_node->linked_to = malloc(sizeof(struct link));
    new_node->linked_to->next = NULL;
    new_node->linked_to->node = NULL;
    new_node->backpropagation = malloc(sizeof(struct link));
    new_node->backpropagation->next = NULL;
    new_node->backpropagation->node = NULL;
    new_node->is_special = 0;
    new_node->cost = number_of_nodes;
    new_node->visited = 0;
    graph = new_node;
    for(i=2; i<=number_of_nodes; i++) {
        new_node->next = malloc(sizeof(struct graph));
        new_node = new_node->next;
        new_node->cost = number_of_nodes;
        new_node->visited = 0;
        new_node->node_number = i;
        new_node->linked_to = malloc(sizeof(struct link));
        new_node->linked_to->next = NULL;
        new_node->linked_to->node = NULL;
        new_node->backpropagation = malloc(sizeof(struct link));
        new_node->backpropagation->next = NULL;
        new_node->backpropagation->node = NULL;
    }
    new_node->next = NULL;
    return graph;
}


/*Backprop - для прохода от конечной в сторону начальной*/
struct graph *address_plus_backprop_init(struct graph *graph, struct graph *from_node, int number)
{
    /*Node=null, next=null => node =...*/
    /*Node=val, next=null => malloc, node = ...*/
    /*Node=val, next=val => gotonext*/
    /*Node=null, next=val => does not exist (means skipped link)*/
    struct link *backprop;
    if(graph->node_number == number) {
        backprop = graph->backpropagation;
        if((backprop->next==NULL)&&(backprop->node==NULL)) {
            backprop->node = from_node;
        }
        else {
            while(backprop->next != NULL) {
                backprop = backprop->next;
            }
            backprop->next = malloc(sizeof(struct link));
            backprop->next->next = NULL;
            backprop->next->node = from_node;
        }
        return graph;
    }
    else {
        return address_plus_backprop_init(graph->next, from_node, number);
    }
}

/*Заполнение буфера нулями*/
void clear_str(char *s) {
    int i;
    for(i=0; i<100; i++) {
        s[i] = 0;
    }
}

/*Новая (прямая) связь в графе*/
void add_link(struct graph *graph, struct link *links, int counter, struct graph *from_node, int to_node)
{
    int i;
    for(i=0; i<counter; i++) {
        if(links->next == NULL) {
            links->next = malloc(sizeof(struct link));
            links->next->next = NULL;
            links->next->node = NULL;
        }
        links = links->next;
    }
    /*Также устанавливаем и предков*/
    links -> node = address_plus_backprop_init(graph, from_node, to_node);
}


/*Построение системы связей особой вершины*/
void parse_n_build_links(struct graph *graph, char *s, int space_counter, struct link *links, struct graph *spec_node)
{
    int i=0, j=0, counter = 0;
    char buffer[100];
    while(counter<space_counter+1) {
        if((s[i] == 32)||(s[i] == 0)) {
            add_link(graph, links, counter, spec_node, atoi(buffer));
            counter++;
            clear_str(buffer);
            j = 0;
        }
        else {
            buffer[j] = s[i];
            j++;
        }
        i++;
    }
}

/*Чтение информации и построение рёбер графа*/
struct graph *build_links(struct graph *graph, int number_of_nodes)
{
    int to, i;
    char c, line[100];
    int space_counter=0, j=0;
    struct graph *cur_node;
    cur_node = graph;
    c = getchar();
    for(i=1; i<=number_of_nodes; i++) {
        clear_str(line);
        space_counter=j=0;
        while((c = getchar()) != 10) {
            if(c==32) {
                space_counter++;
            }
            line[j] = c;
            j++;
        }
        if(space_counter==0) {
            to = atoi(line);
            if(to>0) {
                cur_node->linked_to->node = address_plus_backprop_init(graph, cur_node, to);
            }
        }
        else {
            cur_node->is_special = 1;
            parse_n_build_links(graph, line, space_counter, cur_node->linked_to, cur_node);
        }
        cur_node = cur_node->next;
    }

    return graph;
}

/*Адрес вершины с номером number*/
struct graph *get_raw_address(struct graph *graph, int number)
{
    if(graph->node_number == number) {
        return graph;
    }
    else {
        return get_raw_address(graph->next, number);
    }
}

/*Вычисляем стоимости обратным распространением по предкам от конечной вершины*/
void compute_costs(struct graph *start_node, struct graph *end_node, int accumulated_cost)
{
    struct link *backprop;
    if(!(end_node->visited)) {
        end_node->cost = accumulated_cost;
        end_node->visited = 1;
        if((end_node != start_node) && (end_node->backpropagation->node != NULL)) {
            /*Вызов от всех предков с повышением стоимости*/
            backprop = end_node->backpropagation;
            do {
                if(backprop!=NULL) {
                    if(backprop->node == end_node) {
                        break;
                    }
                }
                compute_costs(start_node, backprop->node, accumulated_cost+1);
                backprop = backprop->next;
            } while(backprop!=NULL);
        }
    }
}

/*Поиск оптимальной длины путей от специальной вершины*/
int choose_min(struct link *children, int best_cost)
{
    if(children!=NULL) {
        if(children->node->cost < best_cost) {
            best_cost = children->node->cost;
        }
        return choose_min(children->next, best_cost);
    }
    else {
        return best_cost;
    }
}

/*Добавление элемента в множество всех вершин, идущих до особой*/
void before_special_append(struct link *before_special, struct graph *node)
{
    if((before_special->next==NULL)&&(before_special->node==NULL)) {
        before_special->node = node;
    }
    else {
        while(before_special->next != NULL) {
            before_special = before_special->next;
        }
        before_special->next = malloc(sizeof(struct link));
        before_special->next->next = NULL;
        before_special->next->node = node;
    }
}

/*Сброс в дерево ответов всех вершин до особой (или всех, если таковой не было)*/
void before_special_print(struct link *before_special, int newline, struct ans_list *ans_tree)
{
    if(ans_tree->ans != 0) {
        while(ans_tree->next != NULL) {
            ans_tree = ans_tree->next;
        }
    }
    while(before_special!=NULL) {
        if(ans_tree->ans == 0) {
            ans_tree->ans = before_special->node->node_number;
        }
        else {
            ans_tree->next = malloc(sizeof(struct ans_list));
            ans_tree->next->next = NULL;
            ans_tree->next->ans = before_special->node->node_number;
            ans_tree = ans_tree->next;
        }
        before_special = before_special->next;
    }
    if(newline) {
        ans_tree->next = malloc(sizeof(struct ans_list));
        ans_tree->next->next = NULL;
        ans_tree->next->ans = -1;
        ans_tree = ans_tree->next;
    }
}

/*Сброс в дерево ответов некоторого одного числа*/
void num_print(int num, int newline, struct ans_list *ans_tree)
{
    if(ans_tree->ans != 0) {
        while(ans_tree->next != NULL) {
            ans_tree = ans_tree->next;
        }
    }

    if(ans_tree->ans == 0) {
        ans_tree->ans = num; 
    }
    else {
        ans_tree->next = malloc(sizeof(struct ans_list));
        ans_tree->next->next = NULL;
        ans_tree->next->ans = num; 
    }

    if(newline) {
        ans_tree = ans_tree->next;
        ans_tree->next = malloc(sizeof(struct ans_list));
        ans_tree->next->next = NULL;
        ans_tree->next->ans = -1; 
    }
}

/*Стоимости уже вычислены. Таким образом, если встречается особая вершина, нам известны
оптимальные пути. Тем самым, мы избавились от возможного перебора с квадратичной
сложностью по времени (в худшем случае)*/
void add_to_path(struct graph *from_node, struct graph *to_node, struct link *before_special, struct ans_list *ans_tree, int spec_visited, int number_of_nodes)
{
    /*от стартовой во все направления, пока не найдена особая или конечная, посещенные записываются куда-то*/
    /*найдена особая: анализ стоимостей и проход по путям с минимальными*/
    /*найдена конечная: печать пути, очистка буфера*/
    /*Вызов от всех потомков*/

    struct link *forwardprop;
    int min_cost;

    /*Хотим заменить принты на добавление в дерево решений*/
    if(!spec_visited) {
        before_special_append(before_special, from_node);
    }

    if((!spec_visited)&&(from_node == to_node)) {
        before_special_print(before_special, 1, ans_tree);
    }

    if(from_node->is_special) {
        spec_visited = 1;
    }

    if((!(from_node->is_special)) && (spec_visited)) {
        if(from_node == to_node) {
            num_print(from_node->node_number, 1, ans_tree);
        }
        else {
            num_print(from_node->node_number, 0, ans_tree);
        }
    }

    if((from_node != to_node) && (from_node->linked_to->node != NULL)) {
        forwardprop = from_node->linked_to;
        min_cost = choose_min(from_node->linked_to, number_of_nodes);
        do {
            if(forwardprop!=NULL) {
                if(forwardprop->node == from_node) {
                    break;
                }
            }
            /*if cost == min*/
            if(forwardprop->node->cost == min_cost) {
                if(from_node->is_special) {
                    before_special_print(before_special, 0, ans_tree);
                }
                add_to_path(forwardprop->node, to_node, before_special, ans_tree, spec_visited, number_of_nodes);
            }
            forwardprop = forwardprop->next;
        } while(forwardprop!=NULL);
    }

}


/*Сброс подсчётов для второй итерации*/
void kill_progress(struct graph *graph, int number_of_nodes)
{
    if(graph != NULL) {
        graph->cost = number_of_nodes;
        graph->visited = 0;
        kill_progress(graph->next, number_of_nodes);
    }
}

/*Печать ответов из дерева ответов*/
void print_ans(struct ans_list *ans_tree)
{
    if(ans_tree->ans == 0) {
        printf("-1\n");
    }
    else {
        do {
            if(ans_tree->ans == -1) {
                printf("\n");
            }
            else {
                printf("%d ", ans_tree->ans);
            }
            ans_tree = ans_tree->next;
        } while(ans_tree != NULL);
    }
}

/*Подсчёт длины каждого пути в дереве*/
int count_len(struct ans_list *ans_tree)
{
    int counter = 0;
    if(ans_tree->ans != 0) {
        do {
            if(ans_tree->ans == -1) {
                break;
            }
            counter++;
            ans_tree = ans_tree->next;
        } while(ans_tree!=NULL);
    }
    return counter;
}

/*Какое из деревьев напечатать? (или, может, вообще никакое...)*/
void analyze_ans(struct ans_list *ans_tree1, struct ans_list *ans_tree2)
{
    int l1 = count_len(ans_tree1), l2 = count_len(ans_tree2);
    printf("\nANSWER:\n");
    if(l1 && !l2) {
        printf("%d\n", l1-1);
        print_ans(ans_tree1);
    }
    else if(l2 && !l1) {
        printf("%d\n", l2-1);
        print_ans(ans_tree2);
    }
    else if(l1>l2) {
        printf("%d\n", l2-1);
        print_ans(ans_tree2);
    }
    else if(l2>l1) {
        printf("%d\n", l1-1);
        print_ans(ans_tree1);
    }
    else if(!l1 && !l2) {
        printf("-1\n");
    }
    else {
        printf("%d\n", l1-1);
        print_ans(ans_tree1);
        print_ans(ans_tree2);
    }
}

/*ANTIMALLOCS*/
void free_link(struct link *link) {
    if(link != NULL) {
        free_link(link->next);
        free(link);
    }
}

void free_graph(struct graph *graph)
{
    if(graph != NULL) {
        free_graph(graph->next);
        free(graph);
    }

}

void free_list(struct ans_list *ans_tree) {
    if(ans_tree != NULL) {
        free_list(ans_tree->next);
        free(ans_tree);
    }
}

void free_links_n_backprops(struct graph *graph)
{
    do
    {
        free_link(graph->backpropagation);
        free_link(graph->linked_to);
        graph = graph->next;
    } while (graph!=NULL);
    
}

/*Использовалась для отладки, в конечном итоге не нужна, но решил оставить*/
void print_graph(struct graph *graph_pointer)
{
    struct link *links;
    printf("Node number %d\n", graph_pointer->node_number);
    printf("Is special? %d\n", graph_pointer->is_special);
    if(!graph_pointer->is_special) {
        if(graph_pointer->linked_to->node == NULL) {
            printf("Linked to None\n");
        }
        else {
            printf("Linked to %d\n", graph_pointer->linked_to->node->node_number);
        }
    }
    else {
        links = graph_pointer->linked_to;
        printf("Linked to ");
        do {
            printf("%d ", links->node->node_number);
            links = links->next;
        } while(links!=NULL);
        printf("\n");
    }
    links = graph_pointer->backpropagation;
    printf("Ancestors ");
    if(links->node != NULL){
        do {
            printf("%d ", links->node->node_number);
            links = links->next;
        } while(links!=NULL);
    }
    else {
        printf("None");
    }
    printf("\n\n");

    if(graph_pointer->next != NULL) {
        print_graph(graph_pointer->next);
    }
}

int main()
{
    int from, to, nn;
    struct graph *from_node, *to_node, *graph;
    struct link *before_special;
    struct ans_list *ans_tree1, *ans_tree2;
    char c;
    printf("Format: <number_of_nodes from_node_number to_node_number>, then node links one-by-line (except special)\n");
    scanf("%d", &nn);
    scanf("%d", &from);
    scanf("%d", &to);

    /*Эту структуру оказалось удобнее создать вне функций*/
    before_special = malloc(sizeof(struct link));
    before_special->next = NULL;
    before_special->node = NULL;

    ans_tree1 = malloc(sizeof(struct ans_list));
    ans_tree1->next = NULL;
    ans_tree1->ans = 0;

    ans_tree2 = malloc(sizeof(struct ans_list));
    ans_tree2->next = NULL;
    ans_tree2->ans = 0;

    graph = init_graph(nn);
    graph = build_links(graph, nn);
    from_node = get_raw_address(graph, from);
    to_node = get_raw_address(graph, to);

    printf("Output graph? [Y/N] ");
    if((c=getchar())=='Y')
        print_graph(graph);

    /*Запуск первой итерации*/
    compute_costs(from_node, to_node, 0);
    if(from_node->cost < nn) {
        add_to_path(from_node, to_node, before_special, ans_tree1, 0, nn);
    }

    /*Подготовка ко второй итерации*/
    kill_progress(graph, nn);
    free_link(before_special);

    /*Не выводить одинаковые ответы дважды*/
    if(to != from) {
        before_special = malloc(sizeof(struct link));
        before_special->next = NULL;
        before_special->node = NULL;

        /*Запуск второй итерации*/
        compute_costs(to_node, from_node, 0);
        if(to_node->cost < nn) {
            add_to_path(to_node, from_node, before_special, ans_tree2, 0, nn);
        }

        free_link(before_special);
    }
    /*Всё, что связано с анализом и выводом ответов*/
    analyze_ans(ans_tree1, ans_tree2);

    /*freeing. Часть из них уже могла быть до этого*/
    free_links_n_backprops(graph);
    free_graph(graph);
    free_list(ans_tree1);
    free_list(ans_tree2);

    return 0;
}