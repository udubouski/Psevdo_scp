extern "C" {
#include "sc_memory_headers.h"
#include "sc_helper.h"
#include "utils.h"
}

#include <stdio.h>
#include <iostream>
#include <glib.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <algorithm>

using namespace std;

sc_memory_context *context;

sc_addr graph, rrel_arcs, rrel_nodes, visit, curr_vertex, father;

sc_bool set_is_not_empty(sc_addr set)
{
    sc_iterator3 *check = sc_iterator3_f_a_a_new(context,
                          set,
                          sc_type_arc_pos_const_perm,
                          0);
    if (SC_TRUE == sc_iterator3_next(check)) {
        return SC_TRUE;
    } else {

        return SC_FALSE;
    }
}

sc_bool find_vertex_in_set(sc_addr element, sc_addr set)
{
    sc_bool find = SC_FALSE;

    sc_iterator3 *location = sc_iterator3_f_a_a_new(context,
                             set,
                             sc_type_arc_pos_const_perm,
                             0);

    while (SC_TRUE == sc_iterator3_next(location)) {
        sc_addr loc = sc_iterator3_value(location, 2);

        if (SC_ADDR_IS_NOT_EQUAL(loc, element)) {
            find = SC_FALSE;
            continue;
        } else {
            find = SC_TRUE;
            break;
        }
    }
    return find;
}

void get_edge_vertexes(sc_addr edge, sc_addr &v1, sc_addr &v2)
{
    sc_memory_get_arc_begin(context, edge, &v1);
    sc_memory_get_arc_end(context, edge, &v2);
}

void print_graph()
{
    sc_addr arcs, nodes, loc, v1, v2, printed_vertex;
    sc_bool find;
    printed_vertex = sc_memory_node_new(context, sc_type_const);

    printEl(context, graph);
    cout << endl << "----------------------" << endl;

    sc_iterator5 *it = sc_iterator5_f_a_a_a_f_new(context,
                       graph,
                       sc_type_arc_pos_const_perm,
                       0,
                       sc_type_arc_pos_const_perm,
                       rrel_arcs);

    if (SC_TRUE == sc_iterator5_next(it)) {
        arcs = sc_iterator5_value(it, 2);

        sc_iterator3 *arcs_it = sc_iterator3_f_a_a_new(context,
                                arcs,
                                sc_type_arc_pos_const_perm,
                                0);

        while (SC_TRUE == sc_iterator3_next(arcs_it)) {

            sc_addr t_arc = sc_iterator3_value(arcs_it, 2);

            get_edge_vertexes(t_arc, v1, v2);

            printEl(context, v1);
            printf(" -- ");
            printEl(context, v2);
            cout << endl;

            if (SC_FALSE == find_vertex_in_set(v1, printed_vertex))
                sc_memory_arc_new(context, sc_type_arc_pos_const_perm, printed_vertex, v1);
            if (SC_FALSE == find_vertex_in_set(v2, printed_vertex))
                sc_memory_arc_new(context, sc_type_arc_pos_const_perm, printed_vertex, v2);
        }
        sc_iterator3_free(arcs_it);
    }

    sc_iterator5_free(it);

    it = sc_iterator5_f_a_a_a_f_new(context,
                                    graph,
                                    sc_type_arc_pos_const_perm,
                                    0,
                                    sc_type_arc_pos_const_perm,
                                    rrel_nodes);

    if (SC_TRUE == sc_iterator5_next(it)) {
        nodes = sc_iterator5_value(it, 2);

        sc_iterator3 *nodes_it = sc_iterator3_f_a_a_new(context,
                                 nodes,
                                 sc_type_arc_pos_const_perm,
                                 0);


        while (SC_TRUE == sc_iterator3_next(nodes_it)) {

            sc_addr t_node = sc_iterator3_value(nodes_it, 2);

            find = find_vertex_in_set(t_node, printed_vertex);

            if (find == SC_FALSE) {
                printEl(context, t_node);
                cout << endl;
            }
        }
        sc_iterator3_free(nodes_it);
    }
    sc_iterator5_free(it);
}

sc_addr get_other_vertex_incidence_edge(sc_addr edge, sc_addr vertex)
{
    sc_addr v1, v2, empty;
    empty.seg = 0;
    empty.offset = 0;

    get_edge_vertexes(edge, v1, v2);
    if ((SC_ADDR_IS_EQUAL(vertex, v1)) || (SC_ADDR_IS_EQUAL(vertex, v2))) {
        if (SC_ADDR_IS_EQUAL(vertex, v1)) {
            return v2;
        } else {
            return v1;
        }
    }
    return empty;
}

/**/
int print_route(sc_addr beg, sc_addr end)
{
    //cout<<'1'<<endl;
    //printEl(context, end);
    int i=0;

    sc_addr curr_vert, curr_ver = end;

    while (true) {
        sc_iterator5 *arcs = sc_iterator5_f_a_a_a_f_new(context,
                             curr_ver,
                             sc_type_arc_common,
                             0,
                             sc_type_arc_pos_const_perm,
                             father);
        if (SC_TRUE == sc_iterator5_next(arcs)) {
            curr_vert = sc_iterator5_value(arcs, 2);
            i++;
            //cout << "<-";
            //printEl(context, curr_vert);
            //cout<<'2'<<endl;
            if (SC_ADDR_IS_EQUAL(curr_vert, beg)) break;
            curr_ver = curr_vert;
            sc_iterator5_free(arcs);
        } else break;
    }
    return i;
}
/**/

sc_addr create_wave(sc_addr wave, sc_addr &not_checked_vertexes)
{
    sc_addr new_wave = sc_memory_node_new(context, sc_type_const);

    sc_iterator3 *it_vertex = sc_iterator3_f_a_a_new(context,
                              wave,
                              sc_type_arc_pos_const_perm,
                              0);

    while (SC_TRUE == sc_iterator3_next(it_vertex)) {
        sc_addr vertex = sc_iterator3_value(it_vertex, 2);

        sc_iterator5 *arcs = sc_iterator5_f_a_a_a_f_new(context,
                             graph,
                             sc_type_arc_pos_const_perm,
                             0,
                             sc_type_arc_pos_const_perm,
                             rrel_arcs);
        if (SC_TRUE == sc_iterator5_next(arcs)) {
            sc_addr set_arcs = sc_iterator5_value(arcs, 2);
            sc_iterator3 *it_arc = sc_iterator3_f_a_a_new(context,
                                   set_arcs,
                                   sc_type_arc_pos_const_perm,
                                   0);
            while (SC_TRUE == sc_iterator3_next(it_arc)) {
                sc_addr t_arc = sc_iterator3_value(it_arc, 2);
                sc_addr other_vertex = get_other_vertex_incidence_edge(t_arc, vertex);

                if (SC_ADDR_IS_EMPTY(other_vertex)) {
                    continue;
                }
                sc_iterator3 *find = sc_iterator3_f_a_f_new(context,
                                     not_checked_vertexes,
                                     sc_type_arc_pos_const_perm,
                                     other_vertex);

                if (SC_TRUE == sc_iterator3_next(find)) {
                    sc_addr edge = sc_iterator3_value(find, 1);
                    sc_memory_element_free(context, edge);
                    sc_memory_arc_new(context, sc_type_arc_pos_const_perm, new_wave, other_vertex);
                    /**/
                    sc_addr boof = sc_memory_arc_new(context, sc_type_arc_common, other_vertex, vertex);
                    sc_memory_arc_new(context, sc_type_arc_pos_const_perm, father, boof);
                    // sc_memory_element_free(boof);
                    /**/
                }
            }
        }
    }

    if (SC_TRUE == set_is_not_empty(new_wave)) {
        return new_wave;
    } else {
        sc_memory_element_free(context, new_wave);
        sc_addr new_wave;
        new_wave.seg = 0;
        new_wave.offset = 0;
        return new_wave;
    }
}


sc_addr find_min_path(sc_addr beg_vertex, sc_addr end_vertex)
{
    sc_addr empty;
    empty.offset = 0;
    empty.seg = 0;

    sc_bool check = SC_FALSE;
    sc_addr not_checked_vertexes = sc_memory_node_new(context, sc_type_const);

    sc_iterator5 *vertexes = sc_iterator5_f_a_a_a_f_new(context,
                             graph,
                             sc_type_arc_pos_const_perm,
                             0,
                             sc_type_arc_pos_const_perm,
                             rrel_nodes);

    if (SC_TRUE == sc_iterator5_next(vertexes)) {
        sc_addr set_vertexes = sc_iterator5_value(vertexes, 2);
        sc_iterator3 *it_vertex = sc_iterator3_f_a_a_new(context,
                                  set_vertexes,
                                  sc_type_arc_pos_const_perm,
                                  0);
        while (SC_TRUE == sc_iterator3_next(it_vertex)) {
            sc_addr curr_vertex = sc_iterator3_value(it_vertex, 2);

            if (SC_ADDR_IS_NOT_EQUAL(curr_vertex, beg_vertex)) {
                if (SC_FALSE == find_vertex_in_set(curr_vertex, not_checked_vertexes))
                    sc_memory_arc_new(context, sc_type_arc_pos_const_perm, not_checked_vertexes, curr_vertex);
            }
        }
    }

    sc_addr new_wave = sc_memory_node_new(context, sc_type_const);
    sc_memory_arc_new(context, sc_type_arc_pos_const_perm, new_wave, beg_vertex);

    sc_addr wave_list = sc_memory_node_new(context, sc_type_const);
    sc_addr wave_list_tail = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, wave_list, new_wave);

    do {
        new_wave = create_wave(new_wave, not_checked_vertexes);


        if (SC_ADDR_IS_EMPTY(new_wave)) {
            sc_memory_element_free(context, wave_list);
            sc_memory_element_free(context, new_wave);
            sc_memory_element_free(context, not_checked_vertexes);
            return empty;
        }

        wave_list_tail = sc_memory_arc_new(context, sc_type_arc_pos_const_perm, wave_list, new_wave);

        sc_iterator3 *find_end = sc_iterator3_f_a_a_new(context,
                                 new_wave,
                                 sc_type_arc_pos_const_perm,
                                 0);

        while (SC_TRUE == sc_iterator3_next(find_end)) {
            sc_addr tmp_vertex = sc_iterator3_value(find_end, 2);
            if (SC_ADDR_IS_EQUAL(tmp_vertex, end_vertex)) {
                check = SC_TRUE;
                continue;
            }
        }

    }

    while (check != SC_TRUE);

    sc_memory_element_free(context, not_checked_vertexes);

    empty = sc_memory_node_new(context, sc_type_const);
    return empty;

}


int showAllNodes()
{
    int r=0;
    sc_iterator5 *it = sc_iterator5_f_a_a_a_f_new(context,

                                    graph,
                                    sc_type_arc_pos_const_perm,
                                    0,
                                    sc_type_arc_pos_const_perm,
                                    rrel_nodes);
    if (SC_TRUE == sc_iterator5_next(it)) {

        sc_addr nodes = sc_iterator5_value(it, 2);

        sc_iterator3 *nodes_it = sc_iterator3_f_a_a_new(context,
                                 nodes,
                                 sc_type_arc_pos_const_perm,
                                 0);
        while (SC_TRUE == sc_iterator3_next(nodes_it)) {
            r++;
            }
        sc_iterator3_free(nodes_it);
        }
    cout << endl;
    sc_iterator5_free(it);
    return r;
}

int max(int* mas,int n)
{
    int max=0;
    for(int i = 0;i<n;i++)
        if(mas[i]>=max)
            max=mas[i];
    return max;
}

void find_exc()
{
    int n=0;
    sc_addr vert1,vert2;
    n=showAllNodes();
    int j;
    sc_iterator5 *vertex = sc_iterator5_f_a_a_a_f_new(context,
                                                      graph,
                                                      sc_type_arc_pos_const_perm,
                                                      0,
                                                      sc_type_arc_pos_const_perm,
                                                      rrel_nodes);
     if (sc_iterator5_next(vertex))
     {
        sc_addr nodes = sc_iterator5_value(vertex, 2);
         sc_iterator3 *it1 = sc_iterator3_f_a_a_new(context,
                                        nodes,
                                        sc_type_arc_pos_const_perm,
                                        0);

          while (sc_iterator3_next(it1))
          {
             int *mas=new int[n];
             j=0;
             sc_iterator3 *it2 = sc_iterator3_f_a_a_new(context,
                                             nodes,
                                             sc_type_arc_pos_const_perm,
                                             0);

              vert1 = sc_iterator3_value(it1, 2);

               while (sc_iterator3_next(it2))
               {
                   vert2 = sc_iterator3_value(it2, 2);
                   if(SC_ADDR_IS_EQUAL(vert1, vert2)) continue;

                   sc_addr lebel = find_min_path(vert1, vert2);

                   if (SC_TRUE == sc_memory_is_element(context, lebel)) {
                       mas[j]=print_route(vert1, vert2);
                       j++;
                       sc_memory_element_free(context, lebel);
                   }
                   else {
                       cout << " doesn't exist" << endl;
                   }
               }
               cout<<"Excentrisitet ";
               printEl(context,vert1);
               cout<<":";
               cout<<max(mas,n)<<endl;
               delete []mas;
            sc_iterator3_free(it2);

          }
          sc_iterator3_free(it1);

     }
     sc_iterator5_free(vertex);
     cout<<"----------------------";
}

void run_test(char number_test)
{
    father = sc_memory_node_new(context, sc_type_const);

    char gr[3] = "Gx";
    gr[1] = number_test;
    sc_helper_resolve_system_identifier(context, gr, &graph);
    sc_helper_resolve_system_identifier(context, "rrel_arcs", &rrel_arcs);
    sc_helper_resolve_system_identifier(context, "rrel_nodes", &rrel_nodes);
    cout << "Graph: ";

    print_graph();
    find_exc();
    cout << endl;
    sc_memory_element_free(context, father);
}


int main()
{
    sc_memory_params params;

    sc_memory_params_clear(&params);
    params.repo_path = "/home/vlad/ostis/kb.bin";
    params.config_file = "/home/vlad/ostis/config/sc-web.ini";
    params.ext_path = "/home/vlad/ostis/sc-machine/bin/extensions";
    params.clear = SC_FALSE;

    sc_memory_initialize(&params);

    context = sc_memory_context_new(sc_access_lvl_make_max);


    //////////////////////////////////////////////////////////////////////////////////
    run_test('0');
    run_test('1');
    run_test('2');
    run_test('3');
    run_test('4');
    cout << "The end" << endl;

    sc_memory_context_free(context);

    sc_memory_shutdown(SC_TRUE);

    return 0;
}
