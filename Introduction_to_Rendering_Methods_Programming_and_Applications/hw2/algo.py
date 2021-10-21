from geometry import Vector, Line
import utils
import numpy as np
import multiprocessing as mp

INTERPOLATION_ENDPOINT = 'endpoint'
INTERPOLATION_CENTER = 'center'

def morphing(src_image, dst_image, T, a, b, p, interpolation):
    assert src_image.data().shape == dst_image.data().shape, 'Size of images mismatch!'

    inter_ctrl_lines = interpolate_control_lines(src_image.ctrl_lines, dst_image.ctrl_lines, T, interpolation)

    trans_src_image = transform_image(src_image, inter_ctrl_lines, a, b, p)
    trans_dst_image = transform_image(dst_image, inter_ctrl_lines, a, b, p)
    out_image = (trans_src_image*(1-T)).astype(trans_src_image.dtype) + (trans_dst_image*(T)).astype(trans_dst_image.dtype)
    #out_image = utils.imgWithCtrlLine(out_image, inter_ctrl_lines)

    return out_image, trans_src_image, trans_dst_image

def transform_image(src_image, dst_ctrl_lines, a, b, p):
    height = src_image.data().shape[0]
    width = src_image.data().shape[1]

    dst_raw_image = np.empty(src_image.data().shape, dtype=src_image.data().dtype)
    dst_vecs = []
    src_vecs = []
    map_arguments = []

    for y in range(height):
        for x in range(width):
            dst_v = Vector((x, y))
            dst_vecs.append(dst_v)
            map_arguments.append((dst_v, dst_ctrl_lines, src_image.ctrl_lines, a, b, p))

    with mp.Pool() as pool:
        src_vecs = pool.starmap(transform_with_multiple_pairs, map_arguments)

    for i in range(len(dst_vecs)):
        dst_vec = dst_vecs[i]
        src_vec = src_vecs[i]
        d_x, d_y = dst_vec.data()
        s_x, s_y = src_vec.data()
        pixel = utils.sample_from_raw_image(s_x, s_y, src_image.data())
        dst_raw_image[d_y][d_x] = pixel

    return dst_raw_image

def transform_with_one_pair(X, l_from, l_to):
    '''
    find p' with respect to l_to from p with respect to l_from
    '''
    P = l_from.f
    Q = l_from.t
    P_ = l_to.f
    Q_ = l_to.t

    u = (X-P).dot((Q-P)) / (Q-P).dot((Q-P))
    v = (X-P).dot((Q-P).perpendicular()) / (Q-P).dot(Q-P)**0.5
    X_ = P_ + (Q_-P_)*u + (Q_-P_).perpendicular()*v / (Q_-P_).dot(Q_-P_)**0.5

    length = (P-Q).dot(P-Q)**0.5

    if u < 0:
        dist = (X-P).dot(X-P)**0.5
    elif u > 1:
        dist = (X-Q).dot(X-Q)**0.5
    else:
        dist = abs(v)

    return X_, length, dist

def transform_with_multiple_pairs(X, lines_from, lines_to, a, b, p):
    assert len(lines_from)==len(lines_to), 'Control lines mismatch!'

    DSUM = Vector((0,0))
    weightsum = 0
    for i in range(len(lines_from)):
        l_from = lines_from[i]
        l_to = lines_to[i]
        X_i, length, dist = transform_with_one_pair(X, l_from, l_to)
        D = X_i - X
        weight = (length**p / (a+dist))**b
        DSUM += D * weight
        weightsum += weight
    X_ = X + DSUM / weightsum

    return X_

def interpolate_control_lines(lines_from, lines_to, T, method):
    assert len(lines_from)==len(lines_to), 'Control lines mismatch!'

    if method == INTERPOLATION_ENDPOINT:
        lines = []
        for i in range(len(lines_from)):
            l_from = lines_from[i]
            l_to = lines_to[i]
            P1, Q1, P2, Q2 = (l_from.f, l_from.t, l_to.f, l_to.t)
            P_ = P1*(1-T) + P2*(T)
            Q_ = Q1*(1-T) + Q2*(T)
            L = Line(P_.round(), Q_.round())
            lines.append(L)
        return lines
    else:
        raise NotImplementedError('Unknow interpolation method')
