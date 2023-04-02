#include "olcPixelGameEngine.h"
#include "item.hpp"
#include "line.hpp"
#include "bin.hpp"
#include "movingitem.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

class StaticPartitionSameSize 
{
public:
    uint32_t bin_width = 128;
	uint32_t bin_height = 64;
	uint32_t number_of_partitions = 6;
	uint32_t total_memory = 0;
	uint32_t used_memory = 0;
	uint32_t internal_fragmentation = 0;
    olc::vi2d upper_left_position = {0, 0};
	std::vector<Bin> bins;

    StaticPartitionSameSize()
    {
        for (uint64_t c = 0; c < number_of_partitions; ++c){
            createNewBin(bin_width, bin_height);
        }
        total_memory = bin_height * bin_width * number_of_partitions;
    }

    StaticPartitionSameSize(uint32_t bin_width, uint32_t bin_height)
        : bin_width(bin_width), bin_height(bin_height)
    {
        for (uint64_t c = 0; c < number_of_partitions; ++c){
            createNewBin(bin_width, bin_height);
        }
        total_memory = bin_height * bin_width * number_of_partitions;
    }
    StaticPartitionSameSize(
        uint32_t bin_width, 
        uint32_t bin_height, 
        uint32_t number_of_partitions
        )
        : 
        bin_width(bin_width), 
        bin_height(bin_height),
        number_of_partitions(number_of_partitions)
    {
        for (uint64_t c = 0; c < number_of_partitions; ++c){
            createNewBin(bin_width, bin_height);
        }
        total_memory = bin_height * bin_width * number_of_partitions;
    }

    StaticPartitionSameSize(StaticPartitionSameSize &other)
    {
        bin_width = other.bin_width;
        bin_height = other.bin_height;
        number_of_partitions = other.number_of_partitions;
        total_memory = other.total_memory;
        used_memory = other.used_memory;
        internal_fragmentation = other.internal_fragmentation;
        
        for (auto &bin : other.bins){
            bins.push_back(bin);
        }
    }

public:
	// First fit iterates trhought all bins and tries to add in all of them
	bool firstFit(Item item, olc::vi2d &insertedPosition){
		if (item.getHeight() > bin_height || item.getWidth() > bin_width) return false;
        for (auto &bin : bins){
			if (bin.getItemsInBin().size() == 0 && bin.insert(item)) {
                used_memory += item.getArea();
                internal_fragmentation += bin.getAreaFree();
				insertedPosition = bin.getItemsInBin()[bin.getItemsInBin().size() - 1].getLeftUpperCorner();
                return true;
			}
		}

		return false;
	}

	bool bestFit(Item item, olc::vi2d &insertedPosition){
        // When all the partition are the same size, the fisrt fit, worst fit and best fit will give the same result
        // Thus I call the first fit because is faster

		return firstFit(item, insertedPosition);
	}

	bool worstFit(Item item, olc::vi2d &insertedPosition){
        // When all the partition are the same size, the fisrt fit, worst fit and best fit will give the same result
        // Thus I call the first fit because is faster

		return firstFit(item, insertedPosition);
	}

    bool removeRandom(){
        uint32_t index = rand() % bins.size();
        uint32_t bins_checked = 0;
        while (bins_checked < bins.size()){
            if (bins[(index + bins_checked) % bins.size()].getAreaUsed() > 0) {
                used_memory -= bins[(index + bins_checked) % bins.size()].getAreaUsed();
                internal_fragmentation -= bins[(index + bins_checked) % bins.size()].getAreaFree();
                bins[(index + bins_checked) % bins.size()].restore();
                return true;
            }
            ++bins_checked;
        }

        return false;
    }

    void clearAll(){
        for (auto &bin : bins){
            bin.restore();
        }
        used_memory = 0;
        internal_fragmentation = 0;
    }

    bool moveByOffset(olc::vi2d offset){
        for (auto &bin : bins) {
            bin.moveItemByOffset(offset);
        }

        upper_left_position += offset;

        return true;
    }

    bool move(olc::vi2d newPosition){
        return moveByOffset( newPosition - upper_left_position);
    }

	bool createNewBin(uint32_t bin_width, uint32_t bin_height ) {
		// Create new Bin
		Bin new_bin = Bin(bin_height, bin_width);
		
		uint32_t offset_between_bins = 1;

		if (bins.size() > 0){
			new_bin.moveItem(olc::vi2d((bins[bins.size() -1].getRightBottomCorner().x + offset_between_bins), 0));
        }
		else {
			new_bin.moveItem(olc::vi2d(0, 0));
		}
		bins.push_back(new_bin);

		return true;
	}
};