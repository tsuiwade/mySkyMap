  
function  h =  Copy_3_of_totalH(point)
    len = length(point);
    k = 1;
    Mdl = KDTreeSearcher(point);
    [n,d] = knnsearch(Mdl,point,'k',k+1);
    distances = d(:,end);
    for i = 1 : length(point)
        if ( point(i,1) < distances(i) ) 
            value = point(i,1) + 360.0;
            [~, dd] = knnsearch(Mdl,[value, point(i, 2)],'k',1);
            distances(i) = min(distances(i), dd);
        elseif (point(i, 1) > 360.0-distances(i) ) 
            value = point(i,1) - 360.0;
            [~, dd] = knnsearch(Mdl,[value, point(i, 2)],'k',1);
            distances(i) = min(distances(i), dd);        
        end
    end
%     distances = zeros([1 len]);
%     delta = points(index(1)) + 360.0 - points(index(end));
%     distances(1) = min(points(index(2)) - points(index(1)), delta);
%     distances(len) = min(points(index(end)) - points(index(end-1)) ,delta );
%     for i = 2 : len - 1
%         distances(i) = min ( points(index(i)) - points(index(i-1)) , points(index(i+1)) - points(index(i)) );
%     end
    d = 2;
    log_c_d = (d/2.) * log(pi) - log(gamma(d/2 + 1));
    sum_log_dist = sum(log(2*distances));
    h = - psi(1) + psi(len) + log_c_d + (d/len) * sum_log_dist;
%     h = sum(log(2*distances));
end