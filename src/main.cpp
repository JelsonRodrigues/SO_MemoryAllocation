#define OLC_PGE_APPLICATION 
#include "include/olcPixelGameEngine.h"
#include "include/utils.hpp"
#include "include/item.hpp"
#include "include/line.hpp"
#include "include/bin.hpp"
#include "include/movingitem.hpp"
#include "include/staticPatitionSameSize.hpp"
#include "include/staticPatitionVariableSize.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

class MemoryAllocation : public olc::PixelGameEngine
{
public:
	uint32_t bins_width = 128;
	uint32_t bins_height = 64;
	StaticPartitionSameSize static_partitions = StaticPartitionSameSize(bins_width, bins_height, 8);
	StaticPartitionVariableSize variable_partitions = StaticPartitionVariableSize(bins_width, bins_height, 10, 5, 3);
	std::vector<Item> items_buffer;
	std::vector<MovingItem> items_buffer_moving_items;
	uint32_t max_Y_value_in_buffer = 0;

	bool toggleHelp = true;

	MemoryAllocation()
	{
		sAppName = "MemoryAllocation";

		// Position the memory in screen
		uint32_t vertical_offset = bins_height + 35;
		static_partitions.move(olc::vi2d(15, ScreenHeight() / 2 + bins_height / 2));
		variable_partitions.move(static_partitions.upper_left_position + olc::vi2d(0, vertical_offset));
	}

public:
	bool Draw(float fElapsedTime)
	{	
		// Background Color
		Clear(olc::Pixel(189, 183, 107));
		// Clear(olc::Pixel(142, 62, 255));
		// Clear(olc::Pixel(104, 159, 85));

		for(auto& bin : static_partitions.bins)
		{
			// Drawing the bin
			olc::vd2d binTLbefore = bin.getLeftUpperCorner() - olc::vi2d(1,1);
			olc::vd2d binBRbefore = bin.getRightBottomCorner() + olc::vi2d(1,1);

			olc::vi2d binTLafter;
			olc::vi2d binBRafter;

			WorldToScreen(binTLbefore, binTLafter);
			WorldToScreen(binBRbefore, binBRafter);

			FillRect(binTLafter, binBRafter - binTLafter, olc::BLACK);
			DrawRect(binTLafter, binBRafter - binTLafter, olc::WHITE);

			// Draw bin usage
			if (vScale.x * bin.getSize().x >= 40.0) {
				olc::vi2d binBL((binTLafter.x + binBRafter.x) / 2.0 - 20, binBRafter.y + 5);
				double percentage = 100.0 * ((double)bin.getArea() - (double)bin.getAreaFree()) / (double)bin.getArea();
				std::stringstream percentageStream;
				percentageStream << std::fixed << std::setprecision(2) << percentage;
				std::string binPercentageUsage = percentageStream.str() + "%";
				DrawString(binBL, binPercentageUsage, olc::YELLOW, 1);
			}

			// Drawing the items inside the bin
			olc::vi2d itemTLafter;
			olc::vi2d itemBRafter;
			for(const auto& item : bin.getItemsInBin())
			{
				WorldToScreen(item.getLeftUpperCorner(), itemTLafter);
				WorldToScreen(item.getRightBottomCorner(), itemBRafter);
				FillRect(itemTLafter, itemBRafter - itemTLafter, item.getColor());
			}
		}

		for(auto& bin : variable_partitions.bins)
		{
			// Drawing the bin
			olc::vd2d binTLbefore = bin.getLeftUpperCorner() - olc::vi2d(1,1);
			olc::vd2d binBRbefore = bin.getRightBottomCorner() + olc::vi2d(1,1);

			olc::vi2d binTLafter;
			olc::vi2d binBRafter;

			WorldToScreen(binTLbefore, binTLafter);
			WorldToScreen(binBRbefore, binBRafter);

			FillRect(binTLafter, binBRafter - binTLafter, olc::BLACK);
			DrawRect(binTLafter, binBRafter - binTLafter, olc::WHITE);

			// Draw bin usage
			if (vScale.x * bin.getSize().x >= 40.0) {
				olc::vi2d binBL((binTLafter.x + binBRafter.x) / 2.0 - 20, binBRafter.y + 5);
				double percentage = 100.0 * ((double)bin.getArea() - (double)bin.getAreaFree()) / (double)bin.getArea();
				std::stringstream percentageStream;
				percentageStream << std::fixed << std::setprecision(2) << percentage;
				std::string binPercentageUsage = percentageStream.str() + "%";
				DrawString(binBL, binPercentageUsage, olc::YELLOW, 1);
			}

			// Drawing the items inside the bin
			olc::vi2d itemTLafter;
			olc::vi2d itemBRafter;
			for(const auto& item : bin.getItemsInBin())
			{
				WorldToScreen(item.getLeftUpperCorner(), itemTLafter);
				WorldToScreen(item.getRightBottomCorner(), itemBRafter);
				FillRect(itemTLafter, itemBRafter - itemTLafter, item.getColor());
			}
		}

		// Draw the buffer of items
		olc::vi2d bufferTLbefore = {0,0};
		olc::vi2d bufferTLafter =  {0,0};
		olc::vi2d bufferBRbefore = {0,0};
		olc::vi2d bufferBRafter =  {0,0};
		if(!items_buffer.empty())
		{
			bufferTLbefore = items_buffer.front().getLeftUpperCorner();
			bufferBRbefore.x = items_buffer.back().getRightBottomCorner().x;
			bufferBRbefore.y = items_buffer.back().getLeftUpperCorner().y + max_Y_value_in_buffer;
			WorldToScreen(bufferTLbefore, bufferTLafter);
			WorldToScreen(bufferBRbefore, bufferBRafter);
			FillRect(bufferTLafter, bufferBRafter - bufferTLafter, olc::BLACK);
			DrawRect(bufferTLafter - olc::vi2d(1,1), (bufferBRafter - bufferTLafter) + olc::vi2d(1,1), olc::WHITE);

			for(const auto& item : items_buffer)
			{
				olc::vi2d itemTLafter;
				olc::vi2d itemBRafter;
				WorldToScreen(item.getLeftUpperCorner(), itemTLafter);
				WorldToScreen(item.getRightBottomCorner(), itemBRafter);
				FillRect(itemTLafter, itemBRafter - itemTLafter, item.getColor());
			}
		}


		// Draw the moving items
		if (!items_buffer_moving_items.empty()){
			// All items are in final position?
			bool final_position_items = true;
			
			for (auto& moving_item : items_buffer_moving_items){
				if (moving_item.isInFinalPosition() == false){
					final_position_items = false;
					moving_item.move(fElapsedTime * 1000.0); 

					olc::vi2d itemTLafter;
					olc::vi2d itemBRafter;
					WorldToScreen(moving_item.getItem().getLeftUpperCorner(), itemTLafter);
					WorldToScreen(moving_item.getItem().getRightBottomCorner(), itemBRafter);
					FillRect(itemTLafter, itemBRafter - itemTLafter, moving_item.getItem().getColor());
				}
			}
			if (final_position_items){
				items_buffer_moving_items.clear();
			}
		}
		
		// Draw the rectangle of creation of an item
		if(vRectDraw)
		{
			olc::vi2d newItemTLafter;
			olc::vi2d newItemBRafter;

			WorldToScreen(vRectStart, newItemTLafter);
			WorldToScreen(vRectNow, newItemBRafter);

			DrawRect(newItemTLafter, newItemBRafter - newItemTLafter, olc::RED);
		}

		return true;
	}

	void OnHandleZoom(float fElapsedTime)
	{
		// Panning and Zoomig, credits to @OneLoneCoder who i'am inpired for
        olc::vd2d vMouse = {(double)GetMouseX(), (double)GetMouseY()};

        // Get the position of the mouse and move the world Final Pos - Inital Pos
        // This make us drag Around the Screen Space, with the OffSet variable
        if(GetMouse(0).bPressed)
        {
            vStartPan = vMouse;
        }

        if(GetMouse(0).bHeld)
        {
            vOffset -= (vMouse - vStartPan) / vScale;
            vStartPan = vMouse;
        }

        olc::vd2d vMouseBeforeZoom;
        ScreenToWorld(vMouse, vMouseBeforeZoom);

		if (GetKey(olc::Key::E).bHeld) 
			vScale *= 1 + (1.1 * fElapsedTime);
		if (GetKey(olc::Key::Q).bHeld) 
			vScale *= 1 - (0.9 * fElapsedTime);
		
		olc::vd2d vMouseAfterZoom;
		ScreenToWorld(vMouse, vMouseAfterZoom);
		vOffset += (vMouseBeforeZoom - vMouseAfterZoom);
	}

	bool OnGui(float fElapsedTime)
	{
        if(toggleHelp)
        {
			olc::Pixel titleColor = olc::GREEN;
			olc::Pixel color = olc::WHITE;
            DrawString(0, 10, "Controles e Ajuda - H:" ,            olc::Pixel(0, 255, 47), 1);
            DrawString(0, 20, "====== Usabilidade ======",          titleColor, 1);
            DrawString(0, 30, "W | A | S | D - Navegacao",          color, 1);
            DrawString(0, 40, "Q | E - Zoom",                       color, 1);
            DrawString(0, 50, "C - Reseta camera",                  color, 1);
            DrawString(0, 60, "Botao Esquerdo - Navegacao",         color, 1);
            DrawString(0, 70, "Botao Direito - Cria item",          color, 1);
            DrawString(0, 90, "====== Atualiza Fila ======",        titleColor, 1);
            DrawString(0, 110, "B | N | M - 1D: G, M, P",           color, 1);
            DrawString(0, 130, "====== Insere Fila ======",         titleColor, 1);
            DrawString(0, 140, "U - Best Fit", color, 1);
            DrawString(0, 150, "I - First Fit", color, 1);
			DrawString(0, 160, "O - Worst Fit", color, 1);
			DrawString(0, 170, "====== Remove Fila ======",         titleColor, 1);
			DrawString(0, 180, "BACK - Remover um item", color, 1);
			DrawString(0, 190, "R - Resetar Memorias", color, 1);
        }

		// Draw the static_paritions stats
		DrawString(ScreenWidth() - 153, 10, "Static Partition", olc::DARK_BLUE, 1);

		std::stringstream percentageStream;
		percentageStream << std::fixed << std::setprecision(2) << (double) static_partitions.used_memory * 100.0 / (double) static_partitions.total_memory;
		std::string memoryPercentageUsage = "Usage: " + percentageStream.str() + "%";
		DrawString(ScreenWidth() - 120, 20, memoryPercentageUsage, olc::YELLOW, 1);

		percentageStream.str(std::string());
		percentageStream << std::fixed << std::setprecision(2) << (double) static_partitions.internal_fragmentation * 100.0 / (double) static_partitions.total_memory;
		std::string fragmentationPercentage = "Internal Fragmentation: " + percentageStream.str() + "%";
		DrawString(ScreenWidth() - 256, 30, fragmentationPercentage, olc::YELLOW, 1);

		// Draw the variable_paritions stats
		DrawString(ScreenWidth() - 168, 40, "Variable Partition", olc::DARK_BLUE, 1);

		percentageStream.str(std::string());
		percentageStream << std::fixed << std::setprecision(2) << (double) variable_partitions.used_memory * 100.0 / (double) variable_partitions.total_memory;
		memoryPercentageUsage = "Usage: " + percentageStream.str() + "%";
		DrawString(ScreenWidth() - 120, 50, memoryPercentageUsage, olc::YELLOW, 1);

		percentageStream.str(std::string());
		percentageStream << std::fixed << std::setprecision(2) << (double) variable_partitions.internal_fragmentation * 100.0 / (double) variable_partitions.total_memory;
		fragmentationPercentage = "Internal Fragmentation: " + percentageStream.str() + "%";
		DrawString(ScreenWidth() - 256, 60, fragmentationPercentage, olc::YELLOW, 1);

        // DrawString(ScreenWidth() - 635, ScreenHeight() - 20, "Jelson Rodrigues - Juathan Duarte - Lucas Morais", olc::Pixel(255,255,255,123), 1);
        // DrawString(ScreenWidth() - 660, ScreenHeight() - 10, "2D BinPacking - github.com/lucaszm7/AED3_Bin_Packing", olc::Pixel(255,255,255,123), 1);
		return true;
	}

	bool OnHandleControls(float fElapsedTime)
	{
		if (GetKey(olc::Key::H).bReleased)
			toggleHelp = !toggleHelp;

		olc::vd2d vMouseScreen = {(double)GetMouseX(), (double)GetMouseY()};
		olc::vd2d vMouseWorld;
        ScreenToWorld(vMouseScreen, vMouseWorld);

		if(GetMouse(1).bPressed)
        {
			vRectDraw = true;
            vRectStart = vMouseWorld;
			vRectNow = vRectStart;
			vRectNow.y += bins_height;
        }

        if(GetMouse(1).bHeld)
        {
            vRectNow.x = vMouseWorld.x;
        }
		
		if (GetKey(olc::Key::A).bHeld)	vOffset -= (olc::vd2d(2.0, 0.0) / vScale) * fElapsedTime * 100;
		if (GetKey(olc::Key::D).bHeld)	vOffset += (olc::vd2d(2.0, 0.0) / vScale) * fElapsedTime * 100;
		if (GetKey(olc::Key::W).bHeld) 	vOffset -= (olc::vd2d(0.0, 2.0) / vScale) * fElapsedTime * 100;
		if (GetKey(olc::Key::S).bHeld)	vOffset += (olc::vd2d(0.0, 2.0) / vScale) * fElapsedTime * 100;

		if (GetKey(olc::Key::R).bPressed) {
			static_partitions.clearAll();
			variable_partitions.clearAll();
		}

		// Add 1d items to the buffer
		if (GetKey(olc::Key::B).bPressed) {
			addItemToBuffer(Item(bins_height,  (rand() % (bins_width / 1) + 1)));
		}
		if (GetKey(olc::Key::N).bPressed) {
			addItemToBuffer(Item(bins_height,  (rand() % (bins_width / 2) + 1)));
		}
		if (GetKey(olc::Key::M).bPressed) {
			addItemToBuffer(Item(bins_height,  (rand() % (bins_width / 4) + 1)));
		}

		// Remove a random item
		if (GetKey(olc::Key::BACK).bPressed) {
			if (static_partitions.removeRandom()){ 
				std::cout << "Removed from static" << "\n";
			}
			if (variable_partitions.removeRandom()){ 
				std::cout << "Removed from variable" << "\n";
			}
		}

		// Add all items in the buffer to the bins with best fit strategy
		if (GetKey(olc::Key::U).bPressed) {
			insert(&MemoryAllocation::bestFitStatic);
			insert(&MemoryAllocation::bestFitVariable);
			items_buffer.clear();
			max_Y_value_in_buffer = 0;
		}

		// Add all items in the buffer to the bins with first fit strategy
		if (GetKey(olc::Key::I).bPressed) {
			insert(&MemoryAllocation::firstFitStatic);
			insert(&MemoryAllocation::firstFitVariable);
			items_buffer.clear();
			max_Y_value_in_buffer = 0;
		}

		// Add all items in the buffer to the bins with worst fit strategy
		if (GetKey(olc::Key::O).bPressed) {
			insert(&MemoryAllocation::worstFitStatic);
			insert(&MemoryAllocation::worstFitVariable);
			items_buffer.clear();
			max_Y_value_in_buffer = 0;
		}

		// Reset screen position
		if (GetKey(olc::Key::C).bPressed) resetScreenPosition();

		if(GetMouse(1).bReleased)
		{
			vRectDraw = false;
			addItemToBuffer(Item(std::abs(vRectStart.y - vRectNow.y), std::abs(vRectStart.x - vRectNow.x)));
			vRectStart = {0,0};
			vRectNow   = {0,0};
		}

		return true;
	}

	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Draw(fElapsedTime);
		OnGui(fElapsedTime);
		OnHandleZoom(fElapsedTime);
		OnHandleControls(fElapsedTime);
		return true;
	}

	bool addItemToBuffer(Item item){
		if (items_buffer.size() == 0) {
			item.moveItem(olc::vi2d(0, -bins_height));
		}
		else {
			item.moveItem(olc::vi2d(items_buffer[items_buffer.size() - 1].getRightBottomCorner().x + 10, -bins_height));	
		}
		items_buffer.push_back(item);
		if (item.getHeight() > max_Y_value_in_buffer) max_Y_value_in_buffer = item.getHeight();
		return true;
	}

	void resetScreenPosition(){
		vOffset = { 0.0, 0.0 };
		vStartPan = { 0.0, 0.0 };
		vScale = { 1.0, 1.0 };
	}

protected:
	bool vRectDraw = false;
	olc::vd2d vRectStart;
	olc::vd2d vRectNow;

protected:
    // Pan & Zoom variables
	olc::vd2d vOffset = { 0.0, 0.0 };
	olc::vd2d vStartPan = { 0.0, 0.0 };
	olc::vd2d vScale = { 1.0, 1.0 };

    void ScreenToWorld(const olc::vi2d& s, olc::vd2d& w)
	{
		w.x = (double)(s.x) / vScale.x + vOffset.x;
		w.y = (double)(s.y) / vScale.y + vOffset.y;
	}

    // Converte coords from Screen Space to World Space
    void WorldToScreen(const olc::vd2d& w, olc::vi2d &s)
	{
		s.x = (int)((w.x - vOffset.x) * vScale.x);
		s.y = (int)((w.y - vOffset.y) * vScale.y);
	}

	// Receive the poiter to the insertion function
	void insert(bool (MemoryAllocation::*insert_function)(Item, olc::vi2d&)){
		for (auto& item: items_buffer){
			olc::vi2d position_before_insert = item.getLeftUpperCorner();
			olc::vi2d inserted_position;
			if ((this->*insert_function)(item, inserted_position)){
				std::cout << "Inserted \n";
				item.moveItem(position_before_insert); //reset the item postition
				items_buffer_moving_items.push_back(MovingItem(item, inserted_position, rand() % 1000 + 500));
			}
			else {
				std::cout << "Fail to insert item \n";
			}
		}
	}

	bool firstFitStatic(Item item, olc::vi2d &insertedPosition){
		return static_partitions.firstFit(item, insertedPosition);
	}
	bool firstFitVariable(Item item, olc::vi2d &insertedPosition){
		return variable_partitions.firstFit(item, insertedPosition);
	}

	bool bestFitStatic(Item item, olc::vi2d &insertedPosition){
		return static_partitions.bestFit(item, insertedPosition);
	}
	bool bestFitVariable(Item item, olc::vi2d &insertedPosition){
		return variable_partitions.bestFit(item, insertedPosition);
	}

	bool worstFitStatic(Item item, olc::vi2d &insertedPosition){
		return static_partitions.worstFit(item, insertedPosition);
	}
	bool worstFitVariable(Item item, olc::vi2d &insertedPosition){
		return variable_partitions.worstFit(item, insertedPosition);
	}
};

int main()
{
	{
		MemoryAllocation app;
		if (app.Construct(640, 360, 2, 2)){
			app.Start();
		}
	}
	return 0;
}