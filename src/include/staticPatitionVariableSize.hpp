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

class StaticPartitionVariableSize 
{
public:
    uint32_t bin_width = 128;
	uint32_t bin_height = 64;
	uint32_t number_of_partitions_small = 8;
	uint32_t number_of_partitions_medium = 4;
	uint32_t number_of_partitions_large = 2;
	uint32_t total_memory = 0;
	uint32_t used_memory = 0;
	uint32_t internal_fragmentation = 0;
    olc::vi2d upper_left_position = {0, 0};
	std::vector<Bin> bins;

    StaticPartitionVariableSize(uint32_t bin_width, uint32_t bin_height)
        : bin_width(bin_width), bin_height(bin_height)
    {
        for (uint64_t c = 0; c < number_of_partitions_small; ++c){
            createNewBin(bin_width / 4, bin_height);
        }
        for (uint64_t c = 0; c < number_of_partitions_medium; ++c){
            createNewBin(bin_width / 2, bin_height);
        }
        for (uint64_t c = 0; c < number_of_partitions_large; ++c){
            createNewBin(bin_width / 1, bin_height);
        }
        total_memory = (bin_height * bin_width / 4) * number_of_partitions_small;
        total_memory += (bin_height * bin_width / 2) * number_of_partitions_medium;
        total_memory += (bin_height * bin_width / 1) * number_of_partitions_large;
    }
    StaticPartitionVariableSize(
        uint32_t bin_width, 
        uint32_t bin_height, 
        uint32_t number_of_partitions_small, 
        uint32_t number_of_partitions_medium,
        uint32_t number_of_partitions_large
        )
        : 
        bin_width(bin_width), 
        bin_height(bin_height),
        number_of_partitions_small(number_of_partitions_small),
        number_of_partitions_medium(number_of_partitions_medium),
        number_of_partitions_large(number_of_partitions_large)
    {
        for (uint64_t c = 0; c < number_of_partitions_small; ++c){
            createNewBin(bin_width / 4, bin_height);
        }
        for (uint64_t c = 0; c < number_of_partitions_medium; ++c){
            createNewBin(bin_width / 2, bin_height);
        }
        for (uint64_t c = 0; c < number_of_partitions_large; ++c){
            createNewBin(bin_width / 1, bin_height);
        }
        total_memory = (bin_height * bin_width / 4) * number_of_partitions_small;
        total_memory += (bin_height * bin_width / 2) * number_of_partitions_medium;
        total_memory += (bin_height * bin_width / 1) * number_of_partitions_large;
    }

    StaticPartitionVariableSize(StaticPartitionVariableSize &other)
    {
        bin_width = other.bin_width;
        bin_height = other.bin_height;
        number_of_partitions_small = other.number_of_partitions_small;
        number_of_partitions_medium = other.number_of_partitions_medium;
        number_of_partitions_large = other.number_of_partitions_large;
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
		for (int c = 0; c < bins.size(); c++){
			if (bins[c].getItemsInBin().size() == 0 && bins[c].insert(item)) {
				used_memory += item.getArea();
                internal_fragmentation += bins[c].getAreaFree();
                insertedPosition = bins[c].getItemsInBin()[bins[c].getItemsInBin().size() - 1].getLeftUpperCorner();
				return true;
			}
		}

		return false;
	}

	// Best fit add to the bin that will have the least amount of space unused after the insertion
	bool bestFit(Item item, olc::vi2d &insertedPosition){
		if (item.getHeight() > bin_height || item.getWidth() > bin_width) return false;

		int64_t index = -1;
		uint64_t min_space_left = UINT64_MAX;
		for (int c = 0; c < bins.size(); c++){
			if (bins[c].getItemsInBin().size() == 0 && bins[c].canFit(item)) {
				if (bins[c].getAreaFree() - item.getArea() < min_space_left){
					min_space_left = bins[c].getAreaFree() - item.getArea();
					index = c;
				}
			}
		}

		if (index >= 0) {
			if (bins[index].insert(item)){
				used_memory += item.getArea();
                internal_fragmentation += bins[index].getAreaFree();
                insertedPosition = bins[index].getItemsInBin()[bins[index].getItemsInBin().size() - 1].getLeftUpperCorner();
				return true;
			}
		}

		return false;
	}

	// Worst fit add to the bin that will have the most amount of space unused after the insertion
	bool worstFit(Item item, olc::vi2d &insertedPosition){
		if (item.getHeight() > bin_height || item.getWidth() > bin_width) return false;

		int64_t index = -1;
		uint64_t max_space_left = 0;
		for (int c = 0; c < bins.size(); c++){
			if (bins[c].getItemsInBin().size() == 0 && bins[c].canFit(item)) {
				if (bins[c].getAreaFree() - item.getArea() > max_space_left){
					max_space_left = bins[c].getAreaFree() - item.getArea();
					index = c;
				}
			}
		}

		if (index >= 0) {
			if (bins[index].insert(item)){
                used_memory += item.getArea();
                internal_fragmentation += bins[index].getAreaFree();
				insertedPosition = bins[index].getItemsInBin()[bins[index].getItemsInBin().size() - 1].getLeftUpperCorner();
				return true;
			}
		}

		return false;
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