/** @file
 * Nagłówek definiujący pomocnicze makro sprawdzające czy wskaźnik jest `NULL`-em.
 * @author Adam Rozenek <adam.rozenek@students.mimuw.edu.pl>
 * @date 12.06.2020
 */

#ifndef ISNULL_H
#define ISNULL_H


/** @brief Sprawdzenie czy wskaźnik jest `NULL`-em.
 * @param[in] ptr           – sprawdzany wskaźnik.
 */
#define ISNULL(ptr) (ptr == NULL)


#endif // ISNULL_H
