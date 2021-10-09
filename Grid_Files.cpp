/*
Vinay - 2020CSM1019
Uday - 2020CSM1007
*/

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <cstdio>
#define T true	//sharing
#define F false //Not sharing

using namespace std;

struct point       //data points, <id> <x-coordinate> <y-coordinate> in dataset file
{
	int id, x, y;
};

struct grid     //structure to store each buckets
{
	string bktName;
	bool sharing;
};



class gridFileSystem    //grid files creation
{
	public:

	int numPoints;
	int bucketSize;
	int numBuckets;
	int minX, maxX, minY, maxY,count;
	int direction;
	enum direction{right, left, up, down};
	set<int> xScales;
	set<int, greater<int>>yScales;
	vector<vector<struct grid>> directory;
	unordered_map<string, pair<int, string>>mapSize;

	gridFileSystem(int numPoints, int bucketSize, int minX, int minY, int maxX, int maxY)   //storing all the necessary info
	{
		this->numPoints = numPoints;
		this->bucketSize = bucketSize;
		this->numBuckets = 0;
		this->xScales.insert(this->minX = minX);
		this->xScales.insert(this->maxX = maxX);
		this->yScales.insert(this->minY = minY);
		this->yScales.insert(this->maxY = maxY);
		struct grid box;
		box.sharing = F;
		vector<struct grid>vect{box};
		this->directory.push_back(vect);
		this->count = 0;
	}

	static int compFn(struct point pt1, struct point pt2)   //to sort the data using id of points for printing while range query output through buckets
	{
		return pt1.id < pt2.id;
	}

	void rangeQuery(int x1, int y1, int x2, int y2)     //range query function to retrieve the data from buckets
	{
		struct point pt1, pt2;
		pt1.x = x1, pt1.y = y1,pt2.x = x2, pt2.y = y2;
		int bottomX, bottomY, topX, topY;
		tie(bottomX, bottomY) = getGridPosition(pt1);
		tie(topX, topY) = getGridPosition(pt2);

		cout<<topX<<" "<<topY<<" "<<bottomX<<" "<<bottomY<<endl;
        cout<<"xScales  : ";
		set<int>::iterator it;
		for(it = xScales.begin(); it != xScales.end(); it++)
			cout<<*it<<", ";
		cout<<"\nyScales : ";
		for(it = yScales.begin(); it != yScales.end(); it++)
			cout<<*it<<", ";
		cout<<endl<<endl;
		cout<<"Retrieved points from buckets: "<<endl;


		unordered_map<string, bool> check;
		vector<struct point> globalData;

		for(int i = topY; i <= bottomY; i++ )
		{
			for(int j = bottomX; j <= topX; j++)
			{
				vector<struct point> data;
				readFile(this->directory[i][j].bktName, data);

				if(check.find(directory[i][j].bktName) != check.end())
                    check[directory[i][j].bktName] = true;
                else
					check[directory[i][j].bktName] = false;

				if(!check[directory[i][j].bktName])
				{
                    cout<<directory[i][j].bktName<<endl;
					for(int k = 0; k < data.size(); k++)
					{
						if((data[k].x >= x1 && data[k].x <=x2) && (data[k].y >= y1 && data[k].y <=y2) )
							globalData.push_back(data[k]);
					}
				}
				check[directory[i][j].bktName] = true;

			}
		}


        for(int i = 0; i < mapSize.size(); i++)
        {
            vector<struct point>vect;
            string name = "overflow"+to_string(i)+".txt";
            readFile(name, vect);
            for(int i = 0; i < vect.size(); i++)
            {
                if((vect[i].x >= x1 && vect[i].x <=x2) && (vect[i].y >= y1 && vect[i].y <=y2) )
							globalData.push_back(vect[i]);
            }
        }

		sort(globalData.begin(), globalData.end(), compFn);

        int sum = 0;
		for(int i = 0; i< globalData.size(); i++)
		{
			cout<<globalData[i].id<<" "<<globalData[i].x<<" "<<globalData[i].y<<endl;
		}
	}


	void buildGrids(string filename)    //grids creation for each points of dataset file
	{
	    cout<<"Running...";
		ifstream input(filename);
		string line;
		struct point pt;
		while(getline(input, line))
		{
			sscanf(line.c_str(), "%d\t%d\t%d", &pt.id, &pt.x, &pt.y);
			this->insertIntoGrids(pt);  //calling insertIntoGrids function to insert into the corresponding grid
		}
	}

	void rangepoints(string filename, int x1, int y1, int x2, int y2)   //this is a naive approach for range query using dataset file
	{
	    ifstream input(filename);
	    string line;
	    struct point pt;
	    while(getline(input, line))
        {
		    sscanf(line.c_str(), "%d\t%d\t%d", &pt.id, &pt.x, &pt.y);
		    if((pt.x >= x1 && pt.x <=x2) && (pt.y >= y1 && pt.y <=y2) )
            {
                cout<<pt.id<<" "<<pt.x<<" "<<pt.y<<endl;
            }
        }
	}


	tuple<int, int> getGridPosition(struct point point2D)   //to get the grid position of given data points
	{
		int xPos, yPos;
		set<int>::iterator it = this->xScales.find(point2D.x);
		if(it == this->xScales.end())
		{
			this->xScales.insert(point2D.x);
			it = this->xScales.find(point2D.x);
			xPos = distance(this->xScales.begin(), it)-1;
			this->xScales.erase(point2D.x);
		}
		else xPos = distance(this->xScales.begin(), it)-1;
		if(xPos == this->xScales.size()-1) xPos--;

		it = this->yScales.find(point2D.y);
		if(it == this->yScales.end())
		{
			this->yScales.insert(point2D.y);
			it = this->yScales.find(point2D.y);
			yPos = distance(this->yScales.begin(), it)-1;
			this->yScales.erase(point2D.y);
		}
		else yPos = distance(this->yScales.begin(), it)-1;
		if(yPos ==this->yScales.size()-1) yPos--;

		if(xPos<0)xPos = 0;
		if(yPos<0)yPos = 0;
		return make_tuple(xPos, yPos);
	}

	tuple<int, int, int> getMedian(string fileName) //this function is to compute median point for splitting
	{
		vector<struct point> bucket;

		readFile(fileName, bucket);
		vector<int> vect1;
		vector<int> vect2;
		int medianX = 1,medianY = 1, mid, axis, flagx=0, flagy=0;

		for(int i=0; i<bucket.size(); i++)
		{
			vect1.push_back(bucket[i].x);
			vect2.push_back(bucket[i].y);
		}

		sort(vect1.begin(), vect1.end());
		sort(vect2.begin(), vect2.end(),greater<int>());


		mid = (bucket.size()-1)/2;
		for(int i = 0; i < vect1.size()-1; i++)
		{
			if(find(this->xScales.begin(), this->xScales.end(), vect1[i]) != this->xScales.end())
				continue;

			if(vect1[i] != vect1[i+1])
			{	medianX = vect1[i]; flagx = 1;}
		}
		if(vect1[mid]  != vect1[vect2.size()-1] && !(find(this->xScales.begin(), this->xScales.end(), vect1[mid]) != this->xScales.end()))
			{
				flagx = 1;
				medianX = vect1[mid];
			}
			else if(vect1[mid]  != vect1[0] && !(find(this->xScales.begin(), this->xScales.end(), vect1[0]) != this->xScales.end()))
			{
				flagx = 1;
				medianX = vect1[0];
			}

		for(int i = 0; i < vect2.size()-1; i++)
		{
			if(find(this->yScales.begin(),this->yScales.end(), vect2[i]) != this->yScales.end())
				continue;

			if(vect2[i] != vect2[i+1])
			{	medianY = vect2[i]; flagy = 1;}
		}
		if(vect2[mid]!=vect2[vect2.size()-1] && !(find(this->yScales.begin(),this->yScales.end(), vect2[mid]) != this->yScales.end()))
			{
				flagy = 1;
				medianY = vect2[mid];
			}
		if(vect2[mid]!=vect2[0] && vect2[mid] !=minY &&!(find(this->yScales.begin(),this->yScales.end(), vect2[0]) != this->yScales.end()))
            {
                flagy = 1;
                medianY = vect2[0];
            }

        if(mid == 0)
        {   if(find(this->yScales.begin(),this->yScales.end(), vect2[0]) != this->yScales.end() && vect2[0] != vect2[1] && vect2[0] !=maxY )
                {   medianY = vect2[0]; flagy = 1;}
        }
		axis = rand()%2;

		if(flagx == 1 && flagy == 1)
		{
			if(axis == 0) return make_tuple(medianX, axis,1);
			else return make_tuple(medianY, axis,1);
		}
		else if(flagx == 1) return make_tuple(medianX, 0,1);
		else if(flagy == 1) return make_tuple(medianY, 1,1);

		return make_tuple(0,0,0);
	}

	void insertIntoGrids(struct point point2D)  //this function inserts given point into appropriate grid and also calls split function if required
	{
		int xPos, yPos, median;
		tie(xPos, yPos) = getGridPosition(point2D);
		if(directory[yPos][xPos].bktName.empty())
		{
			directory[yPos][xPos].bktName = "bucket"+to_string(this->count++)+".txt";
			mapSize[directory[yPos][xPos].bktName].first = 0;
		}
		struct grid *box = &this->directory[yPos][xPos];



		if((this->mapSize[box->bktName].first == this->bucketSize) && (box->sharing == F))//creates grids and splits buckets
		{
			int medianX, medianY, splitAxis, valid;
			tie(median, splitAxis, valid) = this->getMedian(box->bktName);

			if(splitAxis == 0 && valid)
			{
				this->xScales.insert(median);
				this->splitScales(yPos, xPos, splitAxis, median);
			}
			else if(splitAxis == 1 && valid)
			{
				this->yScales.insert(median);
				this->splitScales(yPos, xPos, splitAxis, median);
			}

		}
		else if((this->mapSize[box->bktName].first == this->bucketSize) && (box->sharing == T))//splits buckets
		{

			this->splitSharing(yPos, xPos);
		}

		tie(xPos, yPos) = getGridPosition(point2D);
		box = &directory[yPos][xPos];
		if(mapSize[box->bktName].first== bucketSize)
		{
			addOverflow(box->bktName);
			insertTofile(mapSize[box->bktName].second, point2D);
		}
		else
		{
			insertTofile(box->bktName, point2D);
			mapSize[box->bktName].first+=1;
		}
	}

	void addOverflow(string oldFile)    //if a grid is overflow and points are not able to split into any other grid, storing into a overflow file
	{
		if(mapSize[oldFile].second.empty())
		{
			string num = "" ;
			for(int i = 0; i < oldFile.length(); i++)
			{
				if(oldFile[i] >='0' && oldFile[i] <= '9')
					num+=oldFile[i];
			}
			string overflow = "overflow"+num+".txt";
			mapSize[oldFile].second = overflow;
		}
	}

	void insertTofile(string fileName, struct point point2D)    //this function adds points to the file
	{
		ofstream output;
		output.open(fileName, ios::app);
		output<<point2D.id<<"\t"<<point2D.x<<"\t"<<point2D.y<<endl;
		output.close();

	}

	void readFile(string fileName, vector<struct point> &bucket)    //reading the buckets
	{
		ifstream input;
		input.open(fileName, ios::in);
		string line;

		int id, x, y;
		while(getline(input, line))
		{
			sscanf(line.c_str(), "%d\t%d\t%d", &id, &x, &y);
			bucket.push_back(move(point{id,x,y}));
		}
		input.close();
	}

	void writeFile(string fileName, vector<struct point> &bucket)   //overwrites the bucket with new points after splitting
	{
		ofstream output;
		output.open(fileName, ios::out);
		for(int i=0; i<bucket.size();i++)
			output<<bucket[i].id<<"\t"<<bucket[i].x<<"\t"<<bucket[i].y<<endl;
		output.close();
	}

	void splitScales(int x, int y, int axis, int median) //reconstructs grid array
	{

		if(axis == 1)	// split along row
		{
			vector<struct grid> newGrids = this->directory[x];
			this->directory.insert(directory.begin()+x, newGrids);
			for(int i = 0; i < this->directory[x].size(); i++)
			{
				directory[x][i].sharing = T;	//two rows sharing same bucket
				directory[x+1][i].sharing = T;
				mapSize[directory[x+1][i].bktName].first = mapSize[directory[x][i].bktName].first;
                mapSize[directory[x+1][i].bktName].second = mapSize[directory[x][i].bktName].second;

			}
			this->splitBuckets(x,y,x+1,y, median);
		}
		else		//split along column
		{
			for(int i = 0; i < this->directory.size(); i++)
			{
				this->directory[i].insert(directory[i].begin()+y, directory[i][y]);
				directory[i][y].sharing = T;	//two columns sharing same bucket
				directory[i][y+1].sharing = T;
				mapSize[directory[i][y+1].bktName].first = mapSize[directory[i][y].bktName].first;
                mapSize[directory[i][y+1].bktName].second = mapSize[directory[i][y].bktName].second;
			}
			this->splitBuckets(x,y,x,y+1, median);
		}
	}



	void splitBuckets(int x1,int y1, int x2, int y2, int median)    //to split the buckets whenever required
	{
		int axis = 0;

		directory[x2][y2].bktName = "bucket"+to_string(count++)+".txt";
		mapSize[directory[x2][y2].bktName].first = 0;

		set<int>::iterator itx = this->xScales.begin();
		set<int>::iterator ity = this->yScales.begin();
		if(y1 == y2)
		{
			axis = 1;
		}

		copyBucketsData(directory[x1][y1], directory[x2][y2], axis, median);

		directory[x1][y1].sharing= F;
		directory[x2][y2].sharing= F;

	}


	void splitSharing(int y, int x)     //to split the sharing files
	{

		vector<int>xVect(xScales.begin(), xScales.end());
		vector<int>yVect(yScales.begin(), yScales.end());

		int x1 = x,x2 = x+1,y1 = y+1,y2 = y;
		int bottomX = xVect[x1];
		int bottomY = yVect[y1];
		int topX = xVect[x2];
		int topY = yVect[y2];

		string newFile = "bucket"+to_string(count++)+".txt";
		string oldFile = directory[y][x].bktName;
		//cout<<"bottomx "<<bottomX<<"  bottomy"<<bottomY<<" topX"<<topX<<" topY"<<topY<<endl;


		vector<struct point> oldBucket;
		vector<struct point> newBucket, oldNew;
		readFile(oldFile, oldBucket);
		remove(oldFile.c_str());


		for(int i = 0; i < oldBucket.size(); i++)
		{

			if(oldBucket[i].x > bottomX && oldBucket[i].x <= topX && oldBucket[i].y >= bottomY && oldBucket[i].y < topY )
				newBucket.push_back(oldBucket[i]);
			else
				oldNew.push_back(oldBucket[i]);
		}
        //cout<<"sharing : old size : "<< oldNew.size() << "  new size : "<<newBucket.size()<<endl;
		if(newBucket.size() != 0 && oldNew.size()==0)
		{
			//count--;
            //cout<<"hello"<<endl;
			writeFile(oldFile, oldNew);
            writeFile(newFile, newBucket);

            directory[y][x].bktName = newFile;
            mapSize[newFile].first = newBucket.size();
            mapSize[oldFile].first = 0;
            mapSize[newFile].second = mapSize[oldFile].second;

			int median, splitAxis, valid;
			tie(median, splitAxis, valid) = this->getMedian(newFile);

			if(splitAxis == 0 && valid)
			{
				this->xScales.insert(median);
				this->splitScales(y, x, splitAxis, median);
			}
			else if(splitAxis == 1 && valid)
			{
				this->yScales.insert(median);
				this->splitScales(y, x, splitAxis, median);
			}
			return;
		}

		writeFile(newFile, newBucket);
		writeFile(oldFile, oldNew);
		directory[y][x].bktName = newFile;
		directory[y][x].sharing = F;
		mapSize[oldFile].first = oldNew.size();
		mapSize[newFile].first = newBucket.size();
        mapSize[newFile].second = mapSize[oldFile].second;

	}

	void copyBucketsData(struct grid &grid1, struct grid &grid2 , int axis, int median) //copying bucket data to new buckets whenever it has to split total into a new bucket
	{
		string oldFile = grid1.bktName;
		string newFile = grid2.bktName;
		vector<struct point>oldBucket;
		vector<struct point>newBucket;

		this->readFile(oldFile,oldBucket);
		remove(oldFile.c_str());

		vector<int> indices;
		for(int i = 0; i < oldBucket.size(); i++)
		{
			if(axis == 0 && oldBucket[i].x > median)
			{
				newBucket.push_back(oldBucket[i]);
				indices.push_back(i);
			}
			if(axis == 1 && oldBucket[i].y < median)
			{
				newBucket.push_back(oldBucket[i]);
				indices.push_back(i);
			}

		}

		for(int i=0; i<newBucket.size(); i++)
		{
			indices[i]-=i;
			oldBucket.erase(oldBucket.begin()+indices[i]);
		}
		this->writeFile(oldFile, oldBucket);
		this->writeFile(newFile, newBucket);
		mapSize[grid1.bktName].first = oldBucket.size();
		mapSize[grid2.bktName].first = newBucket.size();
        mapSize[grid1.bktName].second = "";
        mapSize[grid2.bktName].second = "";

	}

	void printGrids()   //printing all the buckets/grids
	{
		cout<<"\n\n"<<endl;
		for(int j = 0; j < this->directory.size(); j++)
		{
			for(int i = 0; i < this->directory[0].size();i++)
			{

				cout<<"Grid : "<<j<<","<<i<<endl;
				struct grid box = this->directory[j][i];

				cout<<box.bktName<<" - size: "<<mapSize[box.bktName].first<< " sharing "<<box.sharing<<endl;
				vector<struct point>bucket;

				readFile(box.bktName, bucket);
				for(int i = 0; i < bucket.size(); i++)
					cout<<"("<<bucket[i].x<<","<<bucket[i].y<<")  ";
				cout<<"\n"<<endl;

			}
		}
	}

};


void generateData(int totalPoints)  //generating data for dataset file
{
	srand(time(0));
	ofstream output("data.txt");
	int lbound = 0, ubound = 400;
	for(int points = 1; points <= totalPoints; points++)
	{
		string line;
		line.append(to_string(points));
		line.append("	");
		line.append(to_string(lbound + rand()%(ubound - lbound + 1)));
		line.append("	");
		line.append(to_string(lbound + rand()%(ubound - lbound + 1)));

		if(points == totalPoints)output<<line;
		else output<<line<<endl;
	}
}

void ranges(int *x, int*y, int minX, int minY, int maxX, int maxY, string m, int n) //range query for the user to give input
{
    do
    {
        int a, b;
        cout<<"\nGive Range for "<<m<<":";
        cout<<"\nX"<<n<<": ";
        cin>>a; *x = a;
        cout<<"\nY"<<n<<": ";
        cin>>b; *y =b;
    }while((*x < minX || *y < minY) || (*x > maxX && *y > maxY));
}

int main()
{
	int count1 =0;
	string name1 = "bucket"+to_string(count1)+".txt";
	string name2 = "overflow"+to_string(count1)+".txt";
	while(!remove(name1.c_str()))
	{
		name2 = "overflow"+to_string(count1)+".txt";
		count1++;
		name1 = "bucket"+to_string(count1)+".txt";
		remove(name2.c_str());
	}

	int numPoints;
	int bucketSize;
	cout<<"Enter the number of data points"<<endl;
	cin>>numPoints;

	cout<<"Enter the Buket Size"<<endl;
	cin>>bucketSize;

	generateData(numPoints);    //calling generateData for dataset creation
	int minX = 0, minY = 0, maxX = 400, maxY = 400;

	gridFileSystem gfsObj(numPoints, bucketSize, minX, minY, maxX, maxY);

	gfsObj.buildGrids("data.txt");  //passing the dataset file to create the grid files
	gfsObj.printGrids();
    while(true){
	cout<<"\nEnter Range Query:"<<endl;
	int x1,y1, x2, y2;
	ranges(&x1, &y1, minX, minY, maxX, maxY, "Lower Left", 1);
	ranges(&x2, &y2, minX, minY, maxX, maxY, "Upper Right", 2);

	gfsObj.rangeQuery(x1,y1, x2, y2);

	cout<<"\n----------------------------------------------\n";
	cout<<"Naive Approach Output:"<<endl;
	gfsObj.rangepoints("data.txt", x1, y1, x2, y2);
    }
	return 0;
}


