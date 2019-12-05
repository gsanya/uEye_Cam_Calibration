%generates points for the intrinsic estimation. It generates points with
%z=0 and it doesn't transform them or anything.

clear all;
numBoards=5;
isChessBoard=true;
if isChessBoard
    vertical=6;
    horizontal=8;
    squareSize=8;
    for(ii=1:vertical)
        for(jj=1:horizontal)
            points((ii-1)*horizontal+jj,:)=[(jj-1)*squareSize,(ii-1)*squareSize,0];
        end
    end
else
    vertical =4;
    horizontal =11;
    pointDistance=8;
     for(ii=1:vertical)
        for(jj=1:horizontal)
            if mod(jj,2)==0 %even
                points((ii-1)*horizontal+jj,:)=[(jj-1)*(pointDistance/2),(ii-1)*pointDistance-(pointDistance/2),0];
            else%odd
                points((ii-1)*horizontal+jj,:)=[(jj-1)*(pointDistance/2),(ii-1)*pointDistance,0];
            end
        end
    end
end
[file,path] = uiputfile('pointsForIntrinsic.txt');
filepath=fullfile(path,file);
fileID = fopen(filepath,'wt');

for ii=1:numBoards
    fprintf(fileID,'board\n');
    for jj=1:size(points,1)
        fprintf(fileID,'[');
        fprintf(fileID,'%d',points(jj,1));
        fprintf(fileID,', ');
        fprintf(fileID,'%d',points(jj,2));
        fprintf(fileID,', ');
        fprintf(fileID,'%d',points(jj,3));
        fprintf(fileID,']\n');
    end
end

fclose(fileID);
%Saving to file:
%TODO: save to file