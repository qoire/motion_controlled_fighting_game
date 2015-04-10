function [ vid_out ] = vga_444_filter( vid_in, width, height )
%DETECTION_FILTER Summary of this function goes here
%   Software model of hardware block
%   Input: frame
%   Output: frame with tracking blocks
    box_size = 12; %24x24 box
    
    clr_counter = 0;
    biggest_clr = 0;
    box_coord = [0 0];
    %secondary processing to set boxes
    for j = 1:(height)
        for k = 1:(width)
            for box_i = -box_size:box_size
                for box_j = -box_size:box_size   
                    % dont operate on boundary conditions
                    if ((j - box_i) < 1 || (j + box_i) > height)
                        break
                    end
                    if ((k - box_j) < 1 || (k + box_j) > width)
                        break
                    end
                    
                    rgb_bit = [0 0 0];  
                    for i = 1:3
                        rgb_bit(i) = bitand(vid_in(j,k,i), 240);
                    end
            
                    %rgb_bit(red, green, blue)
                    if ((rgb_bit(1) >= 150) && (rgb_bit(2) <= 50) && (rgb_bit(3) <= 50))
                        clr_counter = clr_counter + 1;
                    end
                end
            end
            if (clr_counter >= biggest_clr)
                box_coord(1) = j;
                box_coord(2) = k;
                biggest_clr = clr_counter;
            end
            clr_counter = 0;
        end
    end
    
    vid_out = vid_in;    
    for j = (box_coord(1)-box_size):(box_coord(1)+box_size)
        for k = (box_coord(2)-box_size):(box_coord(2)+box_size)
            rgb_bit = [255 0 0];
            
            for i=1:3
                vid_out(j,k,i) = rgb_bit(i);
            end
        end
    end
end

