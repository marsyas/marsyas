#!/usr/bin/env python

import multiprocessing

class EvalManager():
    def __init__(self, num_processes=4):
        """ Sets up infrastructure with the desired number of
        processes."""
        self.pool = multiprocessing.Pool(processes=num_processes)
        self.manager = multiprocessing.Manager()
        self.queue = self.manager.Queue()

    def task(self, function, datalist):
        """ Runs the function on datalist.  Result are available
        via get_results()."""
        for datum in datalist:
            self.pool.apply_async(function, args=(self.queue, datum))

    def task_block(self, function, datalist):
        """ Runs the function on datalist, but blocks.  This is
        very useful for debugging the function.  Result are
        available via get_results()."""
        for datum in datalist:
            apply(function, (self.queue, datum))

    def get_results(self):
        """ Blocks until computation is finished, then returns a
        list of results."""
        self.pool.close()
        self.pool.join()
        results_list = []
        while not self.queue.empty():
            result = self.queue.get()
            results_list.append(result)
        return results_list


