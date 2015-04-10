function [ vid_out ] = detection_filter( vid_in, width, height )
%DETECTION_FILTER Summary of this function goes here
%   Software model of hardware block
%   Input: frame
%   Output: frame with tracking blocks

vid_out = ...
    struct('cdata', zeros(width, height, 3, 'uint8'),...
           'colormap', []);



end

