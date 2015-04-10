function [ vid_out ] = vga_block_filter( vid_in, width, height )
%VGA_BLOCK_FILTER Summary of this function goes here
%   Detailed explanation goes here

%   Software model of hardware block
%   Strategy: Seperate screen into 24 blocks
%   Strategy: light up blocks that have enough red pixels

vid_out = vid_in;

block_size = 12;
block_x = ceil(width/block_size);
block_y = ceil(height/block_size);

for i = 1:block_y
    for j = 1:block_x
        bl_x = j*block_size;
        bl_y = i*block_size;
        
        red_counter = 0;
        green_counter = 0;
        for bl_i = (bl_y+1):(bl_y+block_size)
            for bl_j = (bl_x+1):(bl_x+block_size)
                
                if (bl_i > height || bl_i < 1)
                    break
                end
                
                if (bl_j > width || bl_j < 1)
                    break
                end
                
                rgb_bit = [0 0 0];  
                for color_channel = 1:3
                    rgb_bit(color_channel) = bitand(vid_in(bl_i,bl_j,color_channel), 240);
                end
            
                %rgb_bit(red, green, blue)
                if ((rgb_bit(1) >= 150) && (rgb_bit(2) <= 50) && (rgb_bit(3) <= 50))
                    red_counter = red_counter + 1;
                end
                
                if ((rgb_bit(1) <= 50) && (rgb_bit(2) >= 95) && (rgb_bit(3) <= 125))
                    green_counter = green_counter + 1;
                end
            end
        end
        
        if (red_counter > (12*12)/2)
            for bl_i = (bl_y+1):(bl_y+block_size)
                for bl_j = (bl_x+1):(bl_x+block_size)
                    
                if (bl_i > height || bl_i < 1)
                    break
                end
                
                if (bl_j > width || bl_j < 1)
                    break
                end
                    
                    rgb_bit = [255 0 0];
                    for color_channel = 1:3
                        vid_out(bl_i,bl_j,color_channel) = rgb_bit(color_channel);
                    end
                end
            end
        end
        
        if (green_counter > (12*12)/2)
            for bl_i = (bl_y+1):(bl_y+block_size)
                for bl_j = (bl_x+1):(bl_x+block_size)
                    
                if (bl_i > height || bl_i < 1)
                    break
                end
                
                if (bl_j > width || bl_j < 1)
                    break
                end
                    
                    rgb_bit = [0 255 0];
                    for color_channel = 1:3
                        vid_out(bl_i,bl_j,color_channel) = rgb_bit(color_channel);
                    end
                end
            end
        end
    end
end

end

