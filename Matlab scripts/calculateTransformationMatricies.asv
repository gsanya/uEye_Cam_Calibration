%It calculates the transFormation matricies.
%The assumption is that the calibration pattern is on a 8cm by 8cm cube 
%which is placed on the table, when the table is in the most right position
%(from camera perspective). We place the cube on the table so, that the
%calibration pattern is on the X axis, and a little offset from the middle
%of the coordinate system. We rotate the table with constant increments n
%times in both directions. The bottom left point's coordinate in the
%pattern in the original position (when we only offsetted the pattern, but
%haven't yet roteted the table) is the trans parameter.
function Ret = calculateTransformationMatricies(increment, n, trans)
for ii=-n:n
   R=[cos(ii*increment),-sin(ii*increment),0;
      sin(ii*increment),cos(ii*increment),0;
      0,0,1];
   Ret(:,:,ii+n+1)=[R',R*trans;0,0,0,1];
end