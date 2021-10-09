# 2D-Grid-Filesystem-and-Range-Queries
Storing large dataset of 2D points in memory by organizing them in grids to perform range queries effectively

## Input :  
large dataset of  2-D points  
Range queries : bottom left and top right corner coordinates of a rectangle are given to retrieve all pointa that fall in it

## Output:
2-D points that fall inside the rectange which is given as range query


The algorithm efficiently organizes data in the form of grids. Given a range query rectange the grids that intersect with the rectangle will be loaded into memory which ensures less search time and hence retrieval of the points for the range query will be fast


