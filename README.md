# Bear-ISSRE17
Bear is a framework for statistical analysis of application sensitivity to OS unpredictability that can help developers build more resilient software, discover challenging bugs and identify the scenarios that most need validation. Bear analyzes a program with a set of perturbation strategies on a set of commonly used system calls in order to discover the most sensitive system calls for each application, the most impactful strategies, and how they predict abnormal program outcome.


## Key Features
1. A system call perturbation module ``LKM/``
  	* statically or dynamically hook system calls for adding perturbation strategies
2. A statistical module ``Rcode/``
	* analyze different thresholds and workloads
3. Examples to run the project
	* use examples in ``Logs/`` and see ``Res/`` 


## Environment

* Virturl machine: Linux Ubuntu 12.04
* R studio


## Configuration and Run

To make and install the perturbation module

    cd LKM/ & make
    sudo insmod hook.ko TARGET=app_name stratNo=number callname=syscall
update the ``app_name``, ``number``, and ``syscall`` parameters based on your demands

To run the statistical module, please update the ``path``s with your own data. Possible places to change the `path`.
    
    read.csv(file = 'path',...)
    sink("~/bear/Res/Types/ResProc10_lr2.txt")

    

## Reference
* If you enjoy this work, please cite the following paper 

Sun, Ruimin, et al. "Bear: A framework for understanding application sensitivity to os (mis) behavior." 2016 IEEE 27th International Symposium on Software Reliability Engineering (ISSRE). IEEE, 2016. 

	@inproceedings{sun2016bear,
	  title={Bear: A framework for understanding application sensitivity to os (mis) behavior},
    author={Sun, Ruimin and Lee, Andrew and Chen, Aokun and Porter, Donald E and Bishop, Matt and Oliveira, Daniela},
    booktitle={Software Reliability Engineering (ISSRE), 2016 IEEE 27th International Symposium on Software Reliability Engineering (ISSRE), },
    pages={388--399},
    year={2016},
    organization={IEEE}
	}
  
 
## Contact

Ruimin Sun

gracesrm@ufl.edu
