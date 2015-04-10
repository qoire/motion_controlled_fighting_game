vid_obj = VideoReader('IMG_0503.mp4');

scale_factor = 0.5;
vidHeight = vid_obj.Height*scale_factor;
vidWidth = vid_obj.Width*scale_factor;
MAX_FRAMES = vid_obj.NumberOfFrames;

% Preallocate movie structure.
mov(1:MAX_FRAMES) = ...
    struct('cdata', zeros(vidHeight, vidWidth, 3, 'uint8'),...
           'colormap', []);


for k = 1 : MAX_FRAMES
    vid_buffer = imresize(read(vid_obj, k), scale_factor);
    vid_444_buffer = vga_block_filter(vid_buffer, vidWidth, vidHeight);
    mov(k).cdata = vid_444_buffer;
    print_msg = ['Copying frame: ', num2str(k)];
    disp(print_msg);
end
           
movie2avi(mov, 'myPeaks.avi', 'compression','None', 'fps',vid_obj.framerate);
winopen('myPeaks.avi')

%hf = figure;
%set(hf, 'position', [150 150 vidWidth vidHeight]);

%playmovie
%movie(hf,mov,10,vid_obj.FrameRate);