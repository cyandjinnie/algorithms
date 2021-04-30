#### При помощи минимального остовного дерева мы получим приближение хуже оптимального не более чем в два раза <br>
##### Доказательство: 
* Opt - оптимальное решение, Mst - мин остов
* Ясно что W(Mst) <= W(Opt)
* Пусть V - список вершин, посещаемых при полном обходе => W(V) = 2 * W(Mst) <br> 
* Из предыдущих двух соотношений W(V) <= 2 * W(Opt)
* Пусть H - цикл, который вернул алгоритм, H получается из V иключением повторных <br>
вхождений каждой вершины. По неравенству треугольника (в силу метричности задачи) <br>
стоимость пути при этом не возрастает, тогда W(H) <= W(V) -> W(H) <= 2 * W(Opt)