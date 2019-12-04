%This calculates the Points on the calibration table in global coordinate
%frame.
%The middle of the coordinate system is the middle of the table, when it is
%in the most right (from camera perspective) position. Z is up, X is right
%and parralel with the translational axis. and the pointsOnTable are the
%inner points of the chessboard or the middle points of the assymetric
%circle pattern.
%TransformationMatricies are calculated before.
function Ret = calculatePoints(pointsOnTable, transformationMatricies)
for ii=1:size(transformationMatricies,3)
    for jj=1:size(pointsOnTable,1) 
        tmp=[pointsOnTable(jj,:),1];
        transformationMatricies(:,:,ii);
        tmp2=transformationMatricies(:,:,ii)*tmp';
        Ret(jj,:,ii)=tmp2(1:3)';
    end
end
