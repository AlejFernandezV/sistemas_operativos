/**
 * @file
 * @brief Modulo para generar el diagrama de Gantt de la planificacion.
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef linux
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#endif

#include "list.h"
#include "plot.h"
#include "sched.h"
#include "split.h"
#include "util.h"

int create_plot(char * path, list* processes) {

    int max_time;
    int xticks;
    int nprocesses;

    node_iterator it;
    node_iterator slice_it;
    process * proc;
    slice * s;
    int line_style;

    char * image_path;

    char* plot_cmd;


    image_path = concat(remove_extension(path), ".png");
    plot_cmd = concat("gnuplot \"", concat(path, "\""));

    int line_cnt;
    FILE * stream;

    nprocesses = processes->count;

    if (nprocesses == 0) {
        return 0;
    }

     max_time = max_scheduling_time(processes);

    if (max_time == 0) {
      //Dibujar un grafico vacio con 10 unidades de tiempo
        max_time = 10;
    }

    //Open file to plot command
    stream = fopen(path, "w");

    if (stream == NULL) {
        return 0;
    }

    xticks = 5;

    if (max_time > 100) {
      xticks = 10;
    }

    if (max_time < 20) {
      xticks = 1;
    }

    fprintf(stream, "set term pngcairo dashed size 1024,768\n");
    fprintf(stream, "set output '%s'\n", image_path);
    fprintf(stream, "set style fill solid\n");
    fprintf(stream, "set xrange [0:%d]\n", max_time + (max_time % xticks) + xticks);
    fprintf(stream, "set yrange [0:%d]\n", nprocesses + 1);
    fprintf(stream, "unset ytics\n");
    fprintf(stream, "set title 'Planificacion'\n");
    //fprintf(stream, "set ytics('A' 1,'B' 2, ...)\n");
    fprintf(stream, "set ytics(");
    for (it = head(processes); it != 0; it = next(it)) {
      proc = (process*)it->data;
      fprintf(stream, "'%s' %d", proc->name, proc->pid);
      if (it->next != 0) {
        fprintf(stream, ",");
      }
    }
    fprintf(stream, ")\n");
    fprintf(stream, "set xtics %d\n", xticks);
    fprintf(stream, "unset key\n");
    fprintf(stream, "set xlabel 'Tiempo'\n");
    fprintf(stream, "set ylabel 'Proceso'\n");

    fprintf(stream, "set style line 1 lt 1 lw 2 lc rgb '#00ff00'\n"); //Verde - Tiempo de ejecucion
    fprintf(stream, "set style line 2 lt 1 lw 2 lc rgb '#00ff00'\n"); //Verde - Tiempo de ejecucion
    fprintf(stream, "set style line 3 lt 1 lw 1 lc rgb '#202020'\n"); //Gris claro - Tiempo de espera

    fprintf(stream, "set style arrow 1 heads size screen 0.008,90 ls 1\n");
    fprintf(stream, "set style arrow 2 heads size screen 0.008,100 ls 2\n"); //
    fprintf(stream, "set style arrow 3 heads size screen 0.008,100 ls 3\n"); //

    line_cnt = 1;
    for (it = head(processes); it != 0; it = next(it)) {
        proc = (process*)it->data;
        for (slice_it = head(proc->slices); slice_it != 0; slice_it = next(slice_it)) {
            s = (slice*)slice_it->data;
            line_style = 1;
            if (s->type == WAIT) {
                line_style = 3;
            }
            fprintf(stream, "set arrow %d from %d,%d to %d,%d as %d\n",
                line_cnt++, s->from, proc->pid,
                s->to, proc->pid, line_style);
        }
    }

    //Plot no data (NaN)
    fprintf(stream, "plot NaN\n");
    fclose(stream);

#ifdef linux
  pid_t pid;
  if ( (pid = fork()) < 0) {
    fprintf(stderr, "Warning! unable to execute plot program!\n");
    return 0;
  }

  if (pid == 0){
    execlp("gnuplot", "gnuplot", path, NULL);
    fprintf(stderr, "Warning! error executing plot program!\n");
    exit(EXIT_FAILURE);
  } else {
    int state = 0;
    if (waitpid(pid, &state, 0) != pid) {
      fprintf(stderr, "Warning! unable to plot chart!\n");
      return 0;
    }
  }
#elif _WIN32
  system(plot_cmd);
#elif _WIN64
  system(plot_cmd);
#endif


    return 1;
}



