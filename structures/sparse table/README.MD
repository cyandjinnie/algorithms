#### Алгоритм #####
* Будем использовать структуру данных Sparse table, поддерживая для каждого <br>
отрезка первую и вторую статистики (если отрезок из одного элемента - вторая <br>
статистика для него - бесконечность)
* Время на предпосчет O(nlogn) и время на запрос O(1) следует из свойств <br>
Sparse table

----

#### Доказательство Sparse table ####
##### Построим таблицу по следующему принципу:
* В j-ой (начиная с 0) строке расположены минимумы отрезков длиной 2^j
* При запросе минимума найдем округленный вниз логарифм от длины отрезка (j) <br>
(предпосчитанный чтобы сохранить асимптотику) и вернем минимум от посчитанных <br>
минимумов для двух подотрезков длиной 2^j (они обязательно пересекутся) => <br>
получим верный ответ