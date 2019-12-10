%generate points: up is z axis, right is x axis.
%If chessBoard: bottom left inside corner of the pattern is (0,0,0),
%If assymetric circle pattern: The middle point of the bottom left circle
%is (0,0,0). (The second to left on the bottom will have negative z
%coordinate)
%by visualization the black is the first board, and the blue is the last
%one

%comment needs update

clear all;
isChessBoard=true;
if isChessBoard
    vertical=6;
    horizontal=8;
    squareSize=8;
    for(ii=1:vertical)
        for(jj=1:horizontal)
            points((ii-1)*horizontal+jj,:)=[(jj-1)*squareSize,0,(vertical-ii)*squareSize];
        end
    end
else
    vertical =4;
    horizontal =11;
    pointDistance=8;
     for(ii=1:vertical)
        for(jj=1:horizontal)
            if mod(jj,2)==0 %even
                points((ii-1)*horizontal+jj,:)=[(jj-1)*(pointDistance/2),0,(ii-1)*pointDistance-(pointDistance/2)];
            else%odd
                points((ii-1)*horizontal+jj,:)=[(jj-1)*(pointDistance/2),0,(ii-1)*pointDistance];
            end
        end
    end
end
%calculating the transformation matricies:
TrMats=calculateTransformationMatricies(deg2rad(20),2,[12.382;0;28.019]);
%calculating the points:
GlobPoints=calculatePoints(points,TrMats);

[file,path] = uiputfile('points.txt');
filepath=fullfile(path,file);
fileID = fopen(filepath,'wt');
%print file which we use in c++ program
for ii=1:size(GlobPoints,3)
    fprintf(fileID,'board\n');
    for jj=1:size(GlobPoints(:,:,ii),1)
        fprintf(fileID,'[');
        fprintf(fileID,'%d',GlobPoints(jj,1,ii));
        fprintf(fileID,', ');
        fprintf(fileID,'%d',GlobPoints(jj,2,ii));
        fprintf(fileID,', ');
        fprintf(fileID,'%d',GlobPoints(jj,3,ii));
        fprintf(fileID,']\n');
    end
end

%print file for visualization
[file,path] = uiputfile('points.xyz');
filepath=fullfile(path,file);
fileID = fopen(filepath,'wt');
color=linspace(1,255,size(GlobPoints,3))
color=floor(color)
for ii=1:size(GlobPoints,3)
    for jj=1:size(GlobPoints(:,:,ii),1)
        fprintf(fileID,'%d',GlobPoints(jj,1,ii));
        fprintf(fileID,', ');
        fprintf(fileID,'%d',GlobPoints(jj,2,ii));
        fprintf(fileID,', ');
        fprintf(fileID,'%d',GlobPoints(jj,3,ii));
        fprintf(fileID,', ');
        fprintf(fileID,'%u',0);
        fprintf(fileID,', ');
        fprintf(fileID,'%u',0);
        fprintf(fileID,', ');
        fprintf(fileID,'%u',color(ii));
        fprintf(fileID,'\n');
    end
end
fclose(fileID);
%Saving to file:
%TODO: save to file